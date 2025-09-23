#pragma once
#ifndef EAE6320_GAMEOBJECT_CCAMERA_H
#define EAE6320_GAMEOBJECT_CCAMERA_H

#include "cGameObject.h"

namespace eae6320 {
	namespace GameObject {
		class cCamera : public cGameObject {
		public:
			void Initialize(Math::sVector position, float fov, float near_plane, float far_plane, float speed);

			float GetFOV() { return m_fov; }
			float GetNearPlane() { return m_nearPlane; }
			float GetFarPlane() { return m_farPlane; }
			float GetSpeed() { return m_speed; }
		private:
			float m_fov;
			float m_nearPlane;
			float m_farPlane;
			float m_speed;
		};
	}
}

#endif // EAE6320_GAMEOBJECT_CCAMERA_H
