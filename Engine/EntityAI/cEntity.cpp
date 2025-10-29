#include "cEntity.h"

void eae6320::EntityAI::cEntity::Initialize(Math::sVector position, float speed)
{
	SetSpeed(speed);
	SetPosition(position);
}

void eae6320::EntityAI::cEntity::setMeshAndEffect(Graphics::cMesh* mesh, Graphics::cEffect* effect)
{
	if (mesh) {
		m_mesh = mesh;
		m_mesh->IncrementReferenceCount();
	}
	if (effect) {
		m_effect = effect;
		m_effect->IncrementReferenceCount();
	}
}

void eae6320::EntityAI::cEntity::Rendering(const float i_elapsedSecondCount_sinceLastSimulationUpdate)
{
	if (m_mesh != nullptr && m_effect != nullptr)
		Graphics::CreateGameObject(m_mesh, m_effect,
			GetPhysicsState()->PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate));
}

void eae6320::EntityAI::cEntity::CleanUp() {
	if (m_mesh != nullptr) {
		m_mesh->DecrementReferenceCount();
		m_mesh = nullptr;
	}
	if (m_effect != nullptr) {
		m_effect->DecrementReferenceCount();
		m_effect = nullptr;
	}
}
