#include "cEntity.h"

void eae6320::GameObject::cEntity::Initialize(Math::sVector position, float speed)
{
	m_speed = speed;
	SetPosition(position);
}

void eae6320::GameObject::cEntity::Rendering(Graphics::cMesh* mesh, Graphics::cEffect* effect, const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	Graphics::CreateGameObject(mesh, effect,
		GetPhysicsState()->PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate));
}
