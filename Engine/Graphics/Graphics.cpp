// Includes
//=========

#include "Graphics.h"
#include "cConstantBuffer.h"
#include "ConstantBufferFormats.h"
#include "cRenderState.h"
#include "cShader.h"
#include "cVertexFormat.h"
#include "sContext.h"
#include "VertexFormats.h"
#include "cView.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <Engine/Math/Functions.h>
#include <utility>

// Static Data
//============

namespace
{
	eae6320::Graphics::cView* s_view = new eae6320::Graphics::cView();

	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame);
	eae6320::Graphics::cConstantBuffer s_constantBuffer_drawCall(eae6320::Graphics::ConstantBufferTypes::DrawCall);

	// Submission Data
	//----------------

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sFrame constantData_frame;
		eae6320::Graphics::ConstantBufferFormats::sDrawCall constantData_drawCall[MAX_MEMORY_SIZE];
		float background_color[4];
		eae6320::Graphics::cMesh* meshes[MAX_MEMORY_SIZE];
		eae6320::Graphics::cEffect* effects[MAX_MEMORY_SIZE];
		int numOfPairs;
	};
	// In our class there will be two copies of the data required to render a frame:
	//	* One of them will be in the process of being populated by the data currently being submitted by the application loop thread
	//	* One of them will be fully populated and in the process of being rendered from in the render thread
	// (In other words, one is being produced while the other is being consumed)
	sDataRequiredToRenderAFrame s_dataRequiredToRenderAFrame[2];
	auto* s_dataBeingSubmittedByApplicationThread = &s_dataRequiredToRenderAFrame[0];
	auto* s_dataBeingRenderedByRenderThread = &s_dataRequiredToRenderAFrame[1];
	// The following two events work together to make sure that
	// the main/render thread and the application loop thread can work in parallel but stay in sync:
	// This event is signaled by the application loop thread when it has finished submitting render data for a frame
	// (the main/render thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenAllDataHasBeenSubmittedFromApplicationThread;
	// This event is signaled by the main/render thread when it has swapped render data pointers.
	// This means that the renderer is now working with all the submitted data it needs to render the next frame,
	// and the application loop thread can start submitting data for the following frame
	// (the application loop thread waits for the signal)
	eae6320::Concurrency::cEvent s_whenDataForANewFrameCanBeSubmittedFromApplicationThread;

	// Geometry Data
	//--------------

	//eae6320::Graphics::cMesh* s_mesh = nullptr;
	//eae6320::Graphics::cMesh* s_mesh2 = nullptr;

	// Shading Data
	//-------------

	//eae6320::Graphics::cEffect* s_effect = nullptr;
	//eae6320::Graphics::cEffect* s_effect2 = nullptr;
}

// Helper Declarations
//====================

namespace
{
	void CleanUpMeshAndEffect(sDataRequiredToRenderAFrame* dataRequiredToRenderAFrame);
}

// Interface
//==========

// Submission
//-----------

void eae6320::Graphics::SubmitElapsedTime(const float i_elapsedSecondCount_systemTime, const float i_elapsedSecondCount_simulationTime)
{
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_frame = s_dataBeingSubmittedByApplicationThread->constantData_frame;
	constantData_frame.g_elapsedSecondCount_systemTime = i_elapsedSecondCount_systemTime;
	constantData_frame.g_elapsedSecondCount_simulationTime = i_elapsedSecondCount_simulationTime;
}

void eae6320::Graphics::SetBackgroundColor(float color[4]) {
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& background_color = s_dataBeingSubmittedByApplicationThread->background_color;
	for (int i = 0; i < 4; i++) {
		background_color[i] = color[i];
	}
}

void eae6320::Graphics::CreateGameObject(eae6320::Graphics::cMesh* meshes, eae6320::Graphics::cEffect* effect, Math::sVector position) {
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& meshes_render = s_dataBeingSubmittedByApplicationThread->meshes;
	auto& effects_render = s_dataBeingSubmittedByApplicationThread->effects;
	auto& position_render = s_dataBeingSubmittedByApplicationThread->constantData_drawCall;
	auto& num = s_dataBeingSubmittedByApplicationThread->numOfPairs;

	meshes_render[num] = meshes;
	meshes_render[num]->IncrementReferenceCount();
	effects_render[num] = effect;
	effects_render[num]->IncrementReferenceCount();
	position_render[num].g_transform_localToWorld = Math::cMatrix_transformation(Math::cQuaternion(), position);
	num++;
}

void eae6320::Graphics::SubmitCameraSpace(Math::cQuaternion orientation, Math::sVector position, float fovInDegrees, float nearPlane, float farPlane, float aspectRatio) {
	EAE6320_ASSERT(s_dataBeingSubmittedByApplicationThread);
	auto& constantData_frame = s_dataBeingSubmittedByApplicationThread->constantData_frame;
	constantData_frame.g_transform_worldToCamera = 
		constantData_frame.g_transform_worldToCamera.CreateWorldToCameraTransform(orientation, position);
	constantData_frame.g_transform_cameraToProjected = 
		constantData_frame.g_transform_cameraToProjected.CreateCameraToProjectedTransform_perspective(Math::ConvertDegreesToRadians(fovInDegrees),
			aspectRatio, nearPlane, farPlane);
}

eae6320::cResult eae6320::Graphics::WaitUntilDataForANewFrameCanBeSubmitted(const unsigned int i_timeToWait_inMilliseconds)
{
	return Concurrency::WaitForEvent(s_whenDataForANewFrameCanBeSubmittedFromApplicationThread, i_timeToWait_inMilliseconds);
}

eae6320::cResult eae6320::Graphics::SignalThatAllDataForAFrameHasBeenSubmitted()
{
	return s_whenAllDataHasBeenSubmittedFromApplicationThread.Signal();
}

// Render
//-------

void eae6320::Graphics::RenderFrame()
{
	// Wait for the application loop to submit data to be rendered
	{
		if (Concurrency::WaitForEvent(s_whenAllDataHasBeenSubmittedFromApplicationThread))
		{
			// Switch the render data pointers so that
			// the data that the application just submitted becomes the data that will now be rendered
			std::swap(s_dataBeingSubmittedByApplicationThread, s_dataBeingRenderedByRenderThread);
			// Once the pointers have been swapped the application loop can submit new data
			if (!s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Signal())
			{
				EAE6320_ASSERTF(false, "Couldn't signal that new graphics data can be submitted");
				Logging::OutputError("Failed to signal that new render data can be submitted");
				UserOutput::Print("The renderer failed to signal to the application that new graphics data can be submitted."
					" The application is probably in a bad state and should be exited");
				return;
			}
		}
		else
		{
			EAE6320_ASSERTF(false, "Waiting for the graphics data to be submitted failed");
			Logging::OutputError("Waiting for the application loop to submit data to be rendered failed");
			UserOutput::Print("The renderer failed to wait for the application to submit data to be rendered."
				" The application is probably in a bad state and should be exited");
			return;
		}
	}

	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);
	auto* const dataRequiredToRenderFrame = s_dataBeingRenderedByRenderThread;

	// Update the frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_frame = dataRequiredToRenderFrame->constantData_frame;
		s_constantBuffer_frame.Update(&constantData_frame);
	}

	//Clear the back buffer with RGBA
	{
		auto& background_color = dataRequiredToRenderFrame->background_color;
		s_view->ClearViewBuffers(background_color);
	}

	//Bind shading data and draw mesh
	{
		auto& meshes_render = dataRequiredToRenderFrame->meshes;
		auto& effects_render = dataRequiredToRenderFrame->effects;
		auto& num = dataRequiredToRenderFrame->numOfPairs;
		auto& constantData_drawCall = dataRequiredToRenderFrame->constantData_drawCall;
		for (int i = 0; i < num; i++) {
			effects_render[i]->BindEffect();

			// Update the draw call constant buffer
			s_constantBuffer_drawCall.Update(&constantData_drawCall[i]);

			meshes_render[i]->DrawMesh();
		}
	}

	s_view->SwapViewBuffers();

	// After all of the data that was submitted for this frame has been used
	// you must make sure that it is all cleaned up and cleared out
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	{
		CleanUpMeshAndEffect(dataRequiredToRenderFrame);
	}
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::Graphics::Initialize(const sInitializationParameters& i_initializationParameters)
{
	auto result = Results::Success;

	// Initialize the platform-specific context
	if (!(result = sContext::g_context.Initialize(i_initializationParameters)))
	{
		EAE6320_ASSERTF(false, "Can't initialize Graphics without context");
		return result;
	}
	// Initialize the platform-independent graphics objects
	{
		if (result = s_constantBuffer_frame.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_frame.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without frame constant buffer");
			return result;
		}
	}
	// Initialize the platform-independent draw call objects
	{
		if (result = s_constantBuffer_drawCall.Initialize())
		{
			// There is only a single frame constant buffer that is reused
			// and so it can be bound at initialization time and never unbound
			s_constantBuffer_drawCall.Bind(
				// In our class both vertex and fragment shaders use per-frame constant data
				static_cast<uint_fast8_t>(eShaderType::Vertex) | static_cast<uint_fast8_t>(eShaderType::Fragment));
		}
		else
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without draw call constant buffer");
			return result;
		}
	}
	// Initialize the events
	{
		if (!(result = s_whenAllDataHasBeenSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data has been submitted from the application thread");
			return result;
		}
		if (!(result = s_whenDataForANewFrameCanBeSubmittedFromApplicationThread.Initialize(Concurrency::EventType::ResetAutomaticallyAfterBeingSignaled,
			Concurrency::EventState::Signaled)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without event for when data can be submitted from the application thread");
			return result;
		}
	}
	// Initialize the views
	{
		if (!(result = s_view->Initialize(i_initializationParameters)))
		{
			EAE6320_ASSERTF(false, "Can't initialize Graphics without the views");
			return result;
		}
	}
	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	result = s_view->CleanUp();
	s_view = nullptr;

	{
		CleanUpMeshAndEffect(s_dataBeingSubmittedByApplicationThread);
	}

	{
		CleanUpMeshAndEffect(s_dataBeingRenderedByRenderThread);
	}

	{
		const auto result_constantBuffer_frame = s_constantBuffer_frame.CleanUp();
		if (!result_constantBuffer_frame)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_frame;
			}
		}
	}

	{
		const auto result_constantBuffer_drawCall = s_constantBuffer_drawCall.CleanUp();
		if (!result_constantBuffer_drawCall)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_constantBuffer_drawCall;
			}
		}
	}

	{
		const auto result_context = sContext::g_context.CleanUp();
		if (!result_context)
		{
			EAE6320_ASSERT(false);
			if (result)
			{
				result = result_context;
			}
		}
	}

	return result;
}

// Helper Definitions
//===================

namespace
{
	void CleanUpMeshAndEffect(sDataRequiredToRenderAFrame* dataRequiredToRenderAFrame) {
		auto& meshes_render = dataRequiredToRenderAFrame->meshes;
		auto& effects_render = dataRequiredToRenderAFrame->effects;
		auto& num = dataRequiredToRenderAFrame->numOfPairs;
		for (int i = 0; i < num; i++) {
			effects_render[i]->DecrementReferenceCount();
			effects_render[i] = nullptr;
			meshes_render[i]->DecrementReferenceCount();
			meshes_render[i] = nullptr;
		}
		num = 0;
	}
}