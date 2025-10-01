#pragma once
#ifndef EAE6320_GAMEOBJECT_CCAMERA_H
#define EAE6320_GAMEOBJECT_CCAMERA_H

#include "cGameObject.h"

namespace eae6320 {
	namespace GameObject {
		class cCamera : public cGameObject {
		public:
			void Initialize(Math::sVector position, float fovInDegree, float near_plane, float far_plane, float speed, float aspectRatio = 1.f);
			void Rendering(const float i_elapsedSecondCount_sinceLastSimulationUpdate) final;

			void SetFOV(float fov) { m_fov = fov; }
			void SetNearPlane(float nearPlane) { m_nearPlane = nearPlane; }
			void SetFarPlane(float farPlane) { m_farPlane = farPlane; }
			void SetAspectRatio(float aspectRatio) { m_aspectRatio = aspectRatio; }

		private:
			float m_fov;
			float m_nearPlane;
			float m_farPlane;
			float m_aspectRatio;
		};
	}
}

#endif // EAE6320_GAMEOBJECT_CCAMERA_H
