#include "cCamera.h"
#include "Engine/Graphics/Graphics.h"

void eae6320::GameObject::cCamera::Initialize(Math::sVector position, float fovInDegree, float near_plane, float far_plane, float speed, float aspectRatio) {
	SetPosition(position);
	m_fov = fovInDegree;
	m_nearPlane = near_plane;
	m_farPlane = far_plane;
	m_speed = speed;
	m_aspectRatio = aspectRatio;
}

void eae6320::GameObject::cCamera::RenderCamera(const float i_elapsedSecondCount_sinceLastUpdate)
{
	Graphics::SubmitCameraSpace(GetPhysicsState()->orientation,
		GetPhysicsState()->PredictFuturePosition(i_elapsedSecondCount_sinceLastUpdate),
		m_fov, m_nearPlane, m_farPlane, m_aspectRatio);
}
