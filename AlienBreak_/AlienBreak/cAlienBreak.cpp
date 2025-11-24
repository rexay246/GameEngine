// Includes
//=========

#include "cAlienBreak.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/UserInput/UserInput.h>
#include <Engine/Physics/PhysicsUtil.h>
#include <iostream>
#include <stdio.h>

// Inherited Implementation
//=========================

// Run
//----

void eae6320::cAlienBreak::UpdateBasedOnInput()
{
	// Is the user pressing the ESC key?
	if ( UserInput::IsKeyPressed( UserInput::KeyCodes::Escape ) )
	{
		// Exit the application
		const auto result = Exit( EXIT_SUCCESS );
		EAE6320_ASSERT( result );
	}
}

void eae6320::cAlienBreak::UpdateSimulationBasedOnInput() {
	hideObjects = UserInput::IsKeyPressed(UserInput::KeyCodes::Space);
	changeEffects = UserInput::IsKeyPressed(UserInput::KeyCodes::Enter);

	// Player
	Math::sVector input = { 0, 0, 0 };
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::A)) {
		input.x -= entity.GetSpeed();
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::D)) {
		input.x += entity.GetSpeed();
	}
	Physics::PhysicsBody2D* body = nullptr;
	m_PhysicsWorld->GetBody(0, body);
	std::vector<Physics::PhysicsBody2D*> result;
	if (input != Math::sVector{ 0, 0, 0 }) {
		m_PhysicsWorld->OverlapBox(Util::ToVec2(entity.GetPosition() + input.GetNormalized() * 0.5f), body->Width, body->Height, result);
		if (result.empty() || result[0] == body) {
			entity.SetVelocity(input);
		}
	}
	else {
		entity.SetVelocity({ 0, 0, 0 });
	}
}

void eae6320::cAlienBreak::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) {
	Physics::PhysicsBody2D* body = nullptr;
	m_PhysicsWorld->GetBody(0, body);
	entity.Update(i_elapsedSecondCount_sinceLastUpdate);
	body->MoveTo(Util::ToVec2(entity.GetPosition()));

	camera.Update(i_elapsedSecondCount_sinceLastUpdate);

	enemy->MoveRandomlyBouncing(i_elapsedSecondCount_sinceLastUpdate, m_PhysicsWorld.get());
	enemy->Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cAlienBreak::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime,
	const float i_elapsedSecondCount_sinceLastSimulationUpdate) {

	Graphics::SetBackgroundColor(bgColor);

	entity.setMeshAndEffect(meshes[0], effects[1]);
	entity.Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);
	entity.CleanUp();

	enemy->setMeshAndEffect(meshes[1], effects[0]);
	enemy->Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);
	enemy->CleanUp();

	camera.Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);
	
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cAlienBreak::Initialize()
{
	// Mesh 1
	{
		Graphics::cMesh::Load(meshes[0], "data/Meshes/BreakerMesh.mesh");
		meshCount++;
	}

	// Mesh 2
	{
		Graphics::cMesh::Load(meshes[1], "data/Meshes/BallMesh.mesh");
		meshCount++;
	}

	// Mesh 3
	{
		Graphics::cMesh::Load(meshes[2], "data/Meshes/FloorObject.mesh");
		meshCount++;
	}

	// Effect 1
	{
		Graphics::cEffect::CreateEffect(effects[0], "data/Shaders/Vertex/standard.shader",
			"data/Shaders/Fragment/animatedshader.shader");
		effectCount++;
	}

	// Effect 2
	{
		Graphics::cEffect::CreateEffect(effects[1],
			"data/Shaders/Vertex/standard.shader",
			"data/Shaders/Fragment/standard.shader");
		effectCount++;
	}

	entity.Initialize({ 0, -3, 0 }, 5.f);
	camera.Initialize({ 0,0,10 }, 45.f, 0.1f, 13.f, 5.f);

	EntityAI::cEntityAI::Load(enemy, "data/EntityAI/Ball.eai");

	bgColor[0] = 0.5f;
	bgColor[1] = 0.5f;
	bgColor[2] = 1.0f;
	bgColor[3] = 1.0f;

	m_PhysicsWorld = std::make_unique<Physics::cPhysicsWorld>();
	m_PhysicsWorld->AddBody(Physics::CreateBoxBody(0.8f, 0.5f, 1.f, false, 1.f));
	m_PhysicsWorld->AddBody(Physics::CreateBoxBody(0.5f, 0.5f, 1.f, false, 1.f));

	Physics::PhysicsBody2D* body = nullptr;
	Physics::PhysicsBody2D* enemyBody = nullptr;
	if (m_PhysicsWorld->GetBody(0, body))
		body->MoveTo(Util::ToVec2(entity.GetPosition()));
	if (m_PhysicsWorld->GetBody(1, enemyBody))
		enemyBody->MoveTo(Util::ToVec2(enemy->GetPosition()));
	enemy->body = enemyBody;
	enemy->player = body;

	return Results::Success;
}

eae6320::cResult eae6320::cAlienBreak::CleanUp()
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
