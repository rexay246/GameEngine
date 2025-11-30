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
	player->MovePlayer(input, m_PhysicsWorld.get(), 0, EntityTracker);

	if (UserInput::IsKeyPressed(UserInput::KeyCodes::Space)) {
		ball->Die();
	}
}

void eae6320::cAlienBreak::UpdateSimulationBasedOnTime(const float i_elapsedSecondCount_sinceLastUpdate) {
	player->Update(i_elapsedSecondCount_sinceLastUpdate, m_PhysicsWorld.get(), 0, EntityTracker);
	ball->Update(i_elapsedSecondCount_sinceLastUpdate, m_PhysicsWorld.get(), 1, EntityTracker);

	DeathWall->Update(i_elapsedSecondCount_sinceLastUpdate, m_PhysicsWorld.get(), 4, EntityTracker);

	for (unsigned int i = 0; i < NumOfEnemies; i++) {
		aliens[i]->Update(i_elapsedSecondCount_sinceLastUpdate, m_PhysicsWorld.get(), 5 + i, EntityTracker);
	}

	for (unsigned int i = 0; i < 3; i++) {
		Wall[i]->Update(i_elapsedSecondCount_sinceLastUpdate, m_PhysicsWorld.get(), 2 + i, EntityTracker);
	}

	camera.Update(i_elapsedSecondCount_sinceLastUpdate);
}

void eae6320::cAlienBreak::SubmitDataToBeRendered(const float i_elapsedSecondCount_systemTime,
	const float i_elapsedSecondCount_sinceLastSimulationUpdate) {

	Graphics::SetBackgroundColor(bgColor);

	player->Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);

	ball->Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);

	for (unsigned int i = 0; i < NumOfEnemies; i++) {
		aliens[i]->Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);
	}

	for (unsigned int i = 0; i < 3; i++) {
		Wall[i]->Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);
	}
	DeathWall->Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);

	camera.Rendering(i_elapsedSecondCount_sinceLastSimulationUpdate);
	
}

// Initialize / Clean Up
//----------------------

eae6320::cResult eae6320::cAlienBreak::Initialize()
{
	aliens = new BodyEntity::cAlienBodyEntity*[NumOfEnemies];

	// Mesh 1
	{
		Graphics::cMesh::Load(meshes[0], "data/Meshes/BreakerMesh2.mesh");
		meshCount++;
	}

	// Mesh 2
	{
		Graphics::cMesh::Load(meshes[1], "data/Meshes/BallMesh.mesh");
		meshCount++;
	}

	// Mesh 3
	{
		Graphics::cMesh::Load(meshes[2], "data/Meshes/AlienMesh.mesh");
		meshCount++;
	}

	// Mesh 4
	{
		Graphics::cMesh::Load(meshes[3], "data/Meshes/WallVerticalMesh.mesh");
		meshCount++;
	}

	// Mesh 5
	{
		Graphics::cMesh::Load(meshes[4], "data/Meshes/WallHorizontalMesh.mesh");
		meshCount++;
	}

	// Mesh 6
	{
		Graphics::cMesh::Load(meshes[5], "data/Meshes/AlienMesh2.mesh");
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

	camera.Initialize({ 0,0,10 }, 45.f, 0.1f, 13.f, 5.f);

	bgColor[0] = 0.0f;
	bgColor[1] = 0.0f;
	bgColor[2] = 0.0f;
	bgColor[3] = 1.0f;

	m_PhysicsWorld = std::make_unique<Physics::cPhysicsWorld>();
	m_PhysicsWorld->AddBody(Physics::CreateBoxBody(2.0f, 0.75f, 1.f, false, 1.f));
	m_PhysicsWorld->AddBody(Physics::CreateBoxBody(0.5f, 0.5f, 1.f, false, 1.f));

	Physics::PhysicsBody2D* body = nullptr;
	Physics::PhysicsBody2D* ballBody = nullptr;

	if (m_PhysicsWorld->GetBody(0, body)) {
		player = new BodyEntity::cPlayerBodyEntity({ 0, -3.5, 0 }, 7.5f, body, meshes[0], effects[1]);
		EntityTracker[body] = player;
	}
	if (m_PhysicsWorld->GetBody(1, ballBody)) {
		ball = new BodyEntity::cBallBodyEntity("data/EntityAI/Ball.eai", ballBody, meshes[1], effects[0]);
		EntityTracker[ballBody] = ball;
		ball->player = player;
	}

	Physics::PhysicsBody2D* WallBody[3];
	m_PhysicsWorld->AddBody(Physics::CreateBoxBody(1.5f, 9.f, 1.f, false, 1.f));
	if (m_PhysicsWorld->GetBody(2, WallBody[0])) {
		Wall[0] = new BodyEntity::cWallBodyEntity({ -4, 0, 0 }, 0.f, WallBody[0], meshes[3], effects[0]);
		EntityTracker[WallBody[0]] = Wall[0];
	}
	m_PhysicsWorld->AddBody(Physics::CreateBoxBody(1.5f, 9.f, 1.f, false, 1.f));
	if (m_PhysicsWorld->GetBody(3, WallBody[1])) {
		Wall[1] = new BodyEntity::cWallBodyEntity({ 4, 0, 0 }, 0.f, WallBody[1], meshes[3], effects[0]);
		EntityTracker[WallBody[1]] = Wall[1];
	}
	m_PhysicsWorld->AddBody(Physics::CreateBoxBody(9.f, 1.5f, 1.f, false, 1.f));
	if (m_PhysicsWorld->GetBody(4, WallBody[2])) {
		Wall[2] = new BodyEntity::cWallBodyEntity({ 0, 5, 0 }, 0.f, WallBody[2], meshes[4], effects[0]);
		EntityTracker[WallBody[2]] = Wall[2];
	}

	Physics::PhysicsBody2D* deathBody;
	m_PhysicsWorld->AddBody(Physics::CreateBoxBody(100.f, 2.f, 1.f, false, 1.f));
	if (m_PhysicsWorld->GetBody(5, deathBody)) {
		DeathWall = new BodyEntity::cDeathBodyEntity({ 0, -7, 0 }, 0.f, deathBody, meshes[4], effects[0]);
		EntityTracker[deathBody] = DeathWall;
	}

	Physics::PhysicsBody2D** enemyBody = new Physics::PhysicsBody2D*[NumOfEnemies];
	for (unsigned int i = 0; i < 4; i++) {
		for (unsigned int j = 0; j < NumOfEnemies / 4; j++) {
			m_PhysicsWorld->AddBody(Physics::CreateBoxBody(0.35f, 0.35f, 1.f, false, 1.f));
			if (m_PhysicsWorld->GetBody(6 + NumOfEnemies / 4 * i + j, enemyBody[NumOfEnemies / 4 * i + j])) {
				if (i % 2 == 0) {
					aliens[NumOfEnemies / 4 * i + j] = new BodyEntity::cAlienBodyEntity("data/EntityAI/Alien.eai", 
						enemyBody[NumOfEnemies / 4 * i + j], meshes[2], effects[0]);
					EntityTracker[enemyBody[NumOfEnemies / 4 * i + j]] = aliens[NumOfEnemies / 4 * i + j];
					aliens[NumOfEnemies / 4 * i + j]->MoveTo(aliens[NumOfEnemies / 4 * i + j]->entity->GetPosition() + 
						Math::sVector{ -2 + j * 1.f, i * 1.f, 0.f });
				}
				else {
					aliens[NumOfEnemies / 4 * i + j] = new BodyEntity::cAlienBodyEntity("data/EntityAI/Alien2.eai", 
						enemyBody[NumOfEnemies / 4 * i + j], meshes[2], effects[0]);
					EntityTracker[enemyBody[NumOfEnemies / 4 * i + j]] = aliens[NumOfEnemies / 4 * i + j];
					aliens[NumOfEnemies / 4 * i + j]->MoveTo(aliens[NumOfEnemies / 4 * i + j]->entity->GetPosition() + 
						Math::sVector{ -2 + j * 1.f, i * 1.f, 0.f });
				}
			}
		}
	}

	return Results::Success;
}

eae6320::cResult eae6320::cAlienBreak::CleanUp()
{
	player->CleanUp();
	ball->CleanUp();
	DeathWall->CleanUp();
	for (unsigned int i = 0; i < NumOfEnemies; i++) {
		aliens[i]->CleanUp();
	}
	for (unsigned int i = 0; i < 3; i++) {
		Wall[i]->CleanUp();
	}
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
