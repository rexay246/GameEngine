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
