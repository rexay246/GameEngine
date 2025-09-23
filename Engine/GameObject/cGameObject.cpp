#include "cGameObject.h"

eae6320::Physics::sRigidBodyState* eae6320::GameObject::cGameObject::GetPhysicsState() {
	return m_physicsState;
}

void eae6320::GameObject::cGameObject::SetPosition(Math::sVector position) {
	m_physicsState->position = position;
}

void eae6320::GameObject::cGameObject::SetVelocity(Math::sVector velocity)
{
	m_physicsState->velocity = velocity;
}

eae6320::GameObject::cGameObject::~cGameObject() {
	m_physicsState = nullptr;
}

void eae6320::GameObject::cGameObject::Rendering(const float i_elapsedSecondCount_sinceLastSimulationUpdate) {
	GetPhysicsState()->Update(i_elapsedSecondCount_sinceLastSimulationUpdate);
	SetPosition(GetPhysicsState()->PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate));
}