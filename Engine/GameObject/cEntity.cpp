#include "cEntity.h"

void eae6320::GameObject::cEntity::Initialize(Graphics::VertexFormats::sVertex_mesh* vertexData,
	int vertexCount, uint16_t* indexData, int indexCount,
	std::string vertexShader, std::string fragmentShader,
	Math::sVector position, float speed) {

	m_effect->CreateEffect(m_effect, vertexShader, fragmentShader);
	m_mesh->CreateMesh(m_mesh, vertexData, vertexCount, indexData, indexCount);
	m_speed = speed;
	SetPosition(position);
}

void eae6320::GameObject::cEntity::Rendering(const float i_elapsedSecondCount_sinceLastSimulationUpdate) {
	GetPhysicsState()->Update(i_elapsedSecondCount_sinceLastSimulationUpdate);
	SetPosition(GetPhysicsState()->PredictFuturePosition(i_elapsedSecondCount_sinceLastSimulationUpdate));
	Graphics::CreateGameObject(m_mesh, m_effect);
}

void eae6320::GameObject::cEntity::CleanUp() {
	m_mesh->DecrementReferenceCount();
	m_mesh = nullptr;
	m_effect->DecrementReferenceCount();
	m_effect = nullptr;
}
