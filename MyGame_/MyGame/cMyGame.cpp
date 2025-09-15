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
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space)) {
		// Slows Down the application
		SetSimulationRate(0.1f);
	}
	else {
		// Returns application to normal
		SetSimulationRate(1.0f);
	}
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime,
	const float i_elapsedSecondCount_sinceLastSimulationUpdate) {
	float color[4] = { 0.0, 0.5, 0.0, 1 };
	Graphics::SetBackgroundColor(color);
	Graphics::CreateGameObject(meshTest, effectTest, 1);
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	effectTest[0]->CreateEffect(effectTest[0],
		"data/Shaders/Vertex/standard.shader",
		"data/Shaders/Fragment/animatedshader.shader");

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
	meshTest[0]->CreateMesh(meshTest[0], vertexData, vertexCount, indexData, indexCount);
	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	meshTest[0]->DecrementReferenceCount();
	meshTest[0] = nullptr;
	effectTest[0]->DecrementReferenceCount();
	effectTest[0] = nullptr;
	return Results::Success;
}
