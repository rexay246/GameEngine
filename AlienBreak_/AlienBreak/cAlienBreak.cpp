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
		input.x -= player->entity->GetSpeed();
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::D)) {
		input.x += player->entity->GetSpeed();
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::W)) {
		input.y -= player->entity->GetSpeed();
	}
	if (UserInput::IsKeyPressed(UserInput::KeyCodes::S)) {
		input.y += player->entity->GetSpeed();
	}
	Physics::PhysicsBody2D* body = nullptr;
	m_PhysicsWorld->GetBody(0, body);
	std::vector<Physics::PhysicsBody2D*> result;
	if (input != Math::sVector{ 0, 0, 0 }) {
		m_PhysicsWorld->OverlapBox(Util::ToVec2(player->entity->GetPosition() + input.GetNormalized() * 0.5f), body->Width, body->Height, result);
		if (result.empty() || result[0] == body) {
			player->entity->SetVelocity(input);
		}
	}
	else {
		player->entity->SetVelocity({ 0, 0, 0 });
	}
}

void eae6320::cAlienBreak::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) {
	//Physics::PhysicsBody2D* body = nullptr;
	//m_PhysicsWorld->GetBody(0, body);
	//entity.Update(i_elapsedSecondCount_sinceLastUpdate);
	//body->MoveTo(Util::ToVec2(entity.GetPosition()));
	player->Update(i_elapsedSecondCount_sinceLastUpdate, m_PhysicsWorld.get(), 0, EntityTracker);
	ball->Update(i_elapsedSecondCount_sinceLastUpdate, m_PhysicsWorld.get(), 1, EntityTracker);

	camera.Update(i_elapsedSecondCount_sinceLastUpdate);

	//Physics::PhysicsBody2D* ballBody = nullptr;
	//m_PhysicsWorld->GetBody(1, ballBody);
	//ball->MoveRandomlyBouncing(i_elapsedSecondCount_sinceLastUpdate, m_PhysicsWorld.get());
	//ball->Update(i_elapsedSecondCount_sinceLastUpdate);
	//ballBody->MoveTo(Util::ToVec2(ball->GetPosition()));
}

void eae6320::cAlienBreak::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime,
	const float i_elapsedSecondCount_sinceLastSimulationUpdate) {

	Graphics::SetBackgroundColor(bgColor);

	//entity.setMeshAndEffect(meshes[0], effects[1]);
	//entity.Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);
	player->entity->Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);

	//ball->setMeshAndEffect(meshes[1], effects[0]);
	ball->entity->Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);

	alien->setMeshAndEffect(meshes[2], effects[1]);
	alien->Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);

	//entity.CleanUp();
	//ball->CleanUp();
	alien->CleanUp();

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
		Graphics::cMesh::Load(meshes[2], "data/Meshes/EnemyMesh.mesh");
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

	//entity.Initialize({ 0, -3, 0 }, 5.f);
	camera.Initialize({ 0,0,10 }, 45.f, 0.1f, 13.f, 5.f);

	//EntityAI::cEntityAI::Load(ball, "data/EntityAI/Ball.eai");
	EntityAI::cEntityAI::Load(alien, "data/EntityAI/Alien.eai");

	bgColor[0] = 0.5f;
	bgColor[1] = 0.5f;
	bgColor[2] = 1.0f;
	bgColor[3] = 1.0f;

	m_PhysicsWorld = std::make_unique<Physics::cPhysicsWorld>();
	m_PhysicsWorld->AddBody(Physics::CreateBoxBody(0.8f, 0.5f, 1.f, false, 1.f));
	m_PhysicsWorld->AddBody(Physics::CreateBoxBody(0.5f, 0.5f, 1.f, false, 1.f));
	//m_PhysicsWorld->AddBody(Physics::CreateBoxBody(0.6f, 0.6f, 1.f, false, 1.f));

	Physics::PhysicsBody2D* body = nullptr;
	Physics::PhysicsBody2D* ballBody = nullptr;
	Physics::PhysicsBody2D* enemyBody = nullptr;
	/*if (m_PhysicsWorld->GetBody(0, body))
		body->MoveTo(Util::ToVec2(entity.GetPosition()));*/
	if (m_PhysicsWorld->GetBody(0, body)) {
		player = new BodyEntity::cBodyEntity({ 0, -3, 0 }, 5.f, body, meshes[0], effects[1]);
		EntityTracker[body] = player;
	}
	if (m_PhysicsWorld->GetBody(1, ballBody)) {
		ball = new BodyEntity::cBallBodyEntity("data/EntityAI/Ball.eai", ballBody, meshes[1], effects[0]);
		EntityTracker[ballBody] = ball;
	}
	if (m_PhysicsWorld->GetBody(2, enemyBody)) {
		enemyBody->MoveTo(Util::ToVec2(alien->GetPosition()));
	}
	//alien->body = enemyBody;

	return Results::Success;
}

eae6320::cResult eae6320::cAlienBreak::CleanUp()
{
	player->CleanUp();
	ball->CleanUp();
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
