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
#include "cMesh.h"
#include "cEffect.h"
#include "cView.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Concurrency/cEvent.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Time/Time.h>
#include <Engine/UserOutput/UserOutput.h>
#include <utility>

// Static Data
//============

namespace
{
	eae6320::Graphics::cView* s_view = new eae6320::Graphics::cView();

	// Constant buffer object
	eae6320::Graphics::cConstantBuffer s_constantBuffer_frame(eae6320::Graphics::ConstantBufferTypes::Frame);

	// Submission Data
	//----------------

	// This struct's data is populated at submission time;
	// it must cache whatever is necessary in order to render a frame
	struct sDataRequiredToRenderAFrame
	{
		eae6320::Graphics::ConstantBufferFormats::sFrame constantData_frame;
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

	eae6320::Graphics::cMesh* s_mesh = new eae6320::Graphics::cMesh();
	eae6320::Graphics::cMesh* s_mesh2 = new eae6320::Graphics::cMesh();

	// Shading Data
	//-------------

	eae6320::Graphics::cEffect* s_effect = new eae6320::Graphics::cEffect();
	eae6320::Graphics::cEffect* s_effect2 = new eae6320::Graphics::cEffect();
}

// Helper Declarations
//====================

namespace
{

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

	//Clear the back buffer with RGBA
	float backBufferColor[4] = {0.0f, 0.0f, 0.5f, 1.0f};
	s_view->ClearViewBuffers(backBufferColor);

	EAE6320_ASSERT(s_dataBeingRenderedByRenderThread);
	auto* const dataRequiredToRenderFrame = s_dataBeingRenderedByRenderThread;

	// Update the frame constant buffer
	{
		// Copy the data from the system memory that the application owns to GPU memory
		auto& constantData_frame = dataRequiredToRenderFrame->constantData_frame;
		s_constantBuffer_frame.Update(&constantData_frame);
	}

	// Bind the first shading data
	{
		s_effect->BindEffect();
	}
	// Draw the first geometry
	{
		s_mesh->DrawMesh();
	}
	// Bind the second shading data
	{
		s_effect2->BindEffect();
	}
	// Draw the second geometry
	{
		s_mesh2->DrawMesh();
	}

	s_view->SwapViewBuffers();

	// After all of the data that was submitted for this frame has been used
	// you must make sure that it is all cleaned up and cleared out
	// so that the struct can be re-used (i.e. so that data for a new frame can be submitted to it)
	{
		// (At this point in the class there isn't anything that needs to be cleaned up)
		//dataRequiredToRenderFrame	// TODO
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
	// Initialize the shading data
	{
		// Effect 1
		{
			if (!(result = s_effect->Initialize(
				"data/Shaders/Vertex/standard.shader",
				"data/Shaders/Fragment/animatedshader.shader")))
			{
				EAE6320_ASSERTF(false, "Can't initialize Graphics without the shading data");
				return result;
			}
		}

		// Effect 2
		{
			if (!(result = s_effect2->Initialize(
				"data/Shaders/Vertex/standard.shader",
				"data/Shaders/Fragment/standard.shader")))
			{
				EAE6320_ASSERTF(false, "Can't initialize Graphics without the shading data");
				return result;
			}
		}
	}
	// Initialize the geometry
	{
		// Mesh 1
		{
			eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[] =
			{
				{ 0.0f, 0.0f, 0.0f },
				{ 1.0f, 1.0f, 0.0f },
				{ 1.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f }
			};
			uint16_t indexData[] =
			{
				0,
				1,
				2,
				1,
				0,
				3
			};
			int vertexCount = std::size(vertexData);
			int indexCount = std::size(indexData);
			if (!(result = s_mesh->Initialize(vertexData, vertexCount, indexData, indexCount)))
			{
				EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
				return result;
			}
		}

		// Mesh 2
		{
			eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[] =
			{
				{ -1.0f, -1.0f, 0.0f },
				{ -1.0f, 0.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f }
			};
			uint16_t indexData[] = {
				0,
				1,
				2,
			};
			int vertexCount = std::size(vertexData);
			int indexCount = std::size(indexData);
			if (!(result = s_mesh2->Initialize(vertexData, vertexCount, indexData, indexCount)))
			{
				EAE6320_ASSERTF(false, "Can't initialize Graphics without the geometry data");
				return result;
			}
		}
	}

	return result;
}

eae6320::cResult eae6320::Graphics::CleanUp()
{
	auto result = Results::Success;

	result = s_view->CleanUp();
	s_view = nullptr;

	result = s_mesh->CleanUp();
	s_mesh = nullptr;

	result = s_mesh2->CleanUp();
	s_mesh2 = nullptr;

	result = s_effect->CleanUp();
	s_effect = nullptr;

	result = s_effect2->CleanUp();
	s_effect2 = nullptr;

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

}