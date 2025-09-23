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
	hideObjects = UserInput::IsKeyPressed(UserInput::KeyCodes::Space);
	changeEffects = UserInput::IsKeyPressed(UserInput::KeyCodes::Enter);
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime,
	const float i_elapsedSecondCount_sinceLastSimulationUpdate) {

	float color[4] = { 0.0, 0.5, 0.0, 1 };
	Graphics::SetBackgroundColor(color);

	Graphics::cEffect** usedEffect = effectTest;
	int pairsUsed = numOfPairs;
	if (changeEffects)
		usedEffect = changedEffect;
	if (hideObjects)
		pairsUsed = 1;
	for (int i = 0; i < pairsUsed; i++) {
		Graphics::CreateGameObject(meshTest[i], usedEffect[i]);
	}
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	// GameObject 1
	{
		// Effect 1
		{
			effectTest[0]->CreateEffect(effectTest[0],
				"data/Shaders/Vertex/standard.shader",
				"data/Shaders/Fragment/animatedshader.shader");
		}

		// Effect 2
		{
			effectTest[1]->CreateEffect(effectTest[1],
				"data/Shaders/Vertex/standard.shader",
				"data/Shaders/Fragment/standard.shader");
		}

		// Changed Effect 1
		{
			changedEffect[0]->CreateEffect(changedEffect[0],
				"data/Shaders/Vertex/standard.shader",
				"data/Shaders/Fragment/standard.shader");
		}

		// Changed Effect 2
		{
			changedEffect[1]->CreateEffect(changedEffect[1],
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
			meshTest[0]->CreateMesh(meshTest[0], vertexData, vertexCount, indexData, indexCount);
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
			meshTest[1]->CreateMesh(meshTest[1], vertexData, vertexCount, indexData, indexCount);
		}
	}
	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	for (int i = 0; i < numOfPairs; i++) {
		meshTest[i]->DecrementReferenceCount();
		meshTest[i] = nullptr;
		effectTest[i]->DecrementReferenceCount();
		effectTest[i] = nullptr;
		changedEffect[i]->DecrementReferenceCount();
		changedEffect[i] = nullptr;
	}
	return Results::Success;
}
