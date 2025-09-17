// Includes
//=========

#include "cMyGame.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>

// Inherited Implementation
//=========================

// Run
//----

void eae6320::cMyGame::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

void eae6320::cMyGame::UpdateSimulationBasedOnInput() {
	//if (UserInput::IsKeyPressed(UserInput::KeyCodes::Enter)) {
	//	// Slows Down the application
	//	SetSimulationRate(0.1f);
	//}
	//else {
	//	// Returns application to normal
	//	SetSimulationRate(1.0f);
	//}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space)) {
		hideObjects = true;
	}
	else {
		hideObjects = false;
	}
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime,
	const float i_elapsedSecondCount_sinceLastSimulationUpdate) {
	float color[4] = { 0.0, 0.5, 0.0, 1 };
	Graphics::SetBackgroundColor(color);
	if (hideObjects)
		Graphics::CreateGameObject(meshes_1.meshes, meshes_1.effects, meshes_1.numOfPairs);
	else
		Graphics::CreateGameObject(meshes_2.meshes, meshes_2.effects, meshes_2.numOfPairs);
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	// GameObject 1
	{
		// Effect 1
		{
			meshes_1.effects[0]->CreateEffect(meshes_1.effects[0],
				"data/Shaders/Vertex/standard.shader",
				"data/Shaders/Fragment/animatedshader.shader");
		}

		// Effect 2
		{
			meshes_1.effects[1]->CreateEffect(meshes_1.effects[1],
				"data/Shaders/Vertex/standard.shader",
				"data/Shaders/Fragment/standard.shader");
		}

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
			int vertexCount = static_cast<int>(std::size(vertexData));
			int indexCount = static_cast<int>(std::size(indexData));
			meshes_1.meshes[0]->CreateMesh(meshes_1.meshes[0], vertexData, vertexCount, indexData, indexCount);
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
			int vertexCount = static_cast<int>(std::size(vertexData));
			int indexCount = static_cast<int>(std::size(indexData));
			meshes_1.meshes[1]->CreateMesh(meshes_1.meshes[1], vertexData, vertexCount, indexData, indexCount);
		}
	}

	// GameObject 2
	{
		// Effect 1
		{
			meshes_2.effects[0]->CreateEffect(meshes_2.effects[0],
				"data/Shaders/Vertex/standard.shader",
				"data/Shaders/Fragment/animatedshader.shader");
		}

		// Effect 2
		{
			meshes_2.effects[1]->CreateEffect(meshes_2.effects[1],
				"data/Shaders/Vertex/standard.shader",
				"data/Shaders/Fragment/standard.shader");
		}

		// Mesh 1
		{
			eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[] =
			{
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, 1.0f, 0.0f },
				{ -1.0f, 0.0f, 0.0f },
				{ -1.0f, 1.0f, 0.0f }
			};
			uint16_t indexData[] =
			{
				0,
				2,
				1,
				2,
				3,
				1
			};
			int vertexCount = static_cast<int>(std::size(vertexData));
			int indexCount = static_cast<int>(std::size(indexData));
			meshes_2.meshes[0]->CreateMesh(meshes_2.meshes[0], vertexData, vertexCount, indexData, indexCount);
		}

		// Mesh 2
		{
			eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[] =
			{
				{ 1.0f, -1.0f, 0.0f },
				{ 0.0f, 0.0f, 0.0f },
				{ 0.0f, -1.0f, 0.0f }
			};
			uint16_t indexData[] = {
				0,
				2,
				1,
			};
			int vertexCount = static_cast<int>(std::size(vertexData));
			int indexCount = static_cast<int>(std::size(indexData));
			meshes_2.meshes[1]->CreateMesh(meshes_2.meshes[1], vertexData, vertexCount, indexData, indexCount);
		}
	}

	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	for (int i = 0; i < meshes_1.numOfPairs; i++) {
		meshes_1.meshes[i]->DecrementReferenceCount();
		meshes_1.meshes[i] = nullptr;
		meshes_1.effects[i]->DecrementReferenceCount();
		meshes_1.effects[i] = nullptr;
	}
	for (int i = 0; i < meshes_2.numOfPairs; i++) {
		meshes_2.meshes[i]->DecrementReferenceCount();
		meshes_2.meshes[i] = nullptr;
		meshes_2.effects[i]->DecrementReferenceCount();
		meshes_2.effects[i] = nullptr;
	}
	return Results::Success;
}
