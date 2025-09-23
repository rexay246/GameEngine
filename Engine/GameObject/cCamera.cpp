#include "cCamera.h"

void eae6320::GameObject::cCamera::Initialize(Math::sVector position, float fov, float near_plane, float far_plane, float speed) {
	SetPosition(position);
	m_fov = fov;
	m_nearPlane = near_plane;
	m_farPlane = far_plane;
	m_speed = speed;
}