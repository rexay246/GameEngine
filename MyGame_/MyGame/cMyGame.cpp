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
	hideObjects = UserInput::IsKeyPressed(UserInput::KeyCodes::Space);
	changeEffects = UserInput::IsKeyPressed(UserInput::KeyCodes::Enter);

	// Camera
	Math::sVector input = { 0, 0, 0 };
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Up)) {
		input.y += camera.GetSpeed();
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Down)) {
		input.y -= camera.GetSpeed();
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Left)) {
		input.x -= camera.GetSpeed();
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Right)) {
		input.x += camera.GetSpeed();
	}
	camera.SetVelocity(input);

	// Player
	input = { 0, 0, 0 };
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::W)) {
		input.y += entity.GetSpeed();
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::S)) {
		input.y -= entity.GetSpeed();
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::A)) {
		input.x -= entity.GetSpeed();
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::D)) {
		input.x += entity.GetSpeed();
	}
	entity.SetVelocity(input);
}

void eae6320::cMyGame::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) {
	entity.Update(i_elapsedSecondCount_sinceLastUpdate);
	camera.Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cMyGame::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime,
	const float i_elapsedSecondCount_sinceLastSimulationUpdate) {

	Graphics::SetBackgroundColor(bgColor);

	GameObject::cEntity entity2;
	GameObject::cEntity entity3;

	entity.setMeshAndEffect(meshes[0], effects[0]);
	entity2.setMeshAndEffect(meshes[1], effects[1]);
	entity3.setMeshAndEffect(meshes[2], effects[0]);

	entity.Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);
	entity2.Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);
	entity3.Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);

	camera.Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);

	entity.CleanUp();
	entity2.CleanUp();
	entity3.CleanUp();
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cMyGame::Initialize()
{
	// Mesh 1
	{
		if ( Graphics::cMesh::Load(meshes[0], "data/Meshes/TooManyVertices.mesh") )
			meshCount++;
	}

	// Mesh 2
	{
		if ( Graphics::cMesh::Load(meshes[1], "data/Meshes/SunObject.mesh") )
			meshCount++;
	}

	// Mesh 3
	{
		if ( Graphics::cMesh::Load(meshes[2], "data/Meshes/FloorObject.mesh") )
			meshCount++;
	}

	// Effect 1
	{
		if ( Graphics::cEffect::CreateEffect(effects[0], "data/Shaders/Vertex/standard.shader",
			"data/Shaders/Fragment/animatedshader.shader") )
			effectCount++;
	}

	// Effect 2
	{
		if ( Graphics::cEffect::CreateEffect(effects[1],
			"data/Shaders/Vertex/standard.shader",
			"data/Shaders/Fragment/standard.shader") )
			effectCount++;
	}

	entity.Initialize({ 0, 0, 0 }, 5.f);
	camera.Initialize({ 0,0,10 }, 45.f, 0.1f, 13.f, 5.f);

	bgColor[0] = 0.5f;
	bgColor[1] = 0.5f;
	bgColor[2] = 1.0f;
	bgColor[3] = 1.0f;

	return Results::Success;
}

eae6320::cResult eae6320::cMyGame::CleanUp()
{
	entity.CleanUp();
	for (int i = 0; i < meshCount; i++) {
		if (meshes[i]) {
			meshes[i]->DecrementReferenceCount();
			meshes[i] = nullptr;
		}
	}
	for (int i = 0; i < effectCount; i++) {
		if (effects[i]) {
			effects[i]->DecrementReferenceCount();
			effects[i] = nullptr;
		}
	}
	return Results::Success;
}
