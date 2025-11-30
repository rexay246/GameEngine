#pragma once
#ifndef EAE6320_ENTITYAI_CAMERABODYENTITY_H
#define EAE6320_ENTITYAI_CAMERABODYENTITY_H

#include "BodyEntity.h"

namespace eae6320 {
	namespace BodyEntity {
		class cCameraBodyEntity : public cBodyEntity {
		public:
			BodyEntity::BodyType type = BodyEntity::Camera;
			bool Activate = false;

			cCameraBodyEntity(Math::sVector position, float speed, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) :
				cBodyEntity(position, speed, _body, mesh_, effect_) {
			}

			cCameraBodyEntity(std::string path, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) :
				cBodyEntity(path, _body, mesh_, effect_) {
				entity->body = body;
			}

			cCameraBodyEntity(Math::sVector position, float speed, float fovInDegree, float near_plane, float far_plane, float aspectRatio = 1.0f) :
				cBodyEntity(position, speed, nullptr, nullptr, nullptr) {
				m_fov = fovInDegree;
				m_nearPlane = near_plane;
				m_farPlane = far_plane;
				m_aspectRatio = aspectRatio;
				entity->Loop = false;

				Math::sVector* cameraShake = new Math::sVector[4];
				float strength = 0.025f;
				cameraShake[0] = { strength, strength, 0 };
				cameraShake[1] = { -strength * 2, -strength * 2, 0 };
				cameraShake[2] = { strength, strength, 0 };
				entity->SetPatrolPoints(cameraShake, 4);
				entity->AcceptanceRadius = 0.01f;
				entity->SetMaxPatrolWaitTime(0.0f);
			}

			void Update(float i_elapsedSecondCount_sinceLastUpdate) {
				if (Activate) {
					if (!entity->Navigate(i_elapsedSecondCount_sinceLastUpdate)) {
						Activate = false;
					}
				}

				if (entity != nullptr) {
					entity->Update(i_elapsedSecondCount_sinceLastUpdate);
				}
			}

			void Rendering(float i_elapsedSecondCount_sinceLastUpdate) override {
				Graphics::SubmitCameraSpace(entity->GetPhysicsState()->orientation,
					entity->GetPhysicsState()->PredictFuturePosition(i_elapsedSecondCount_sinceLastUpdate),
					m_fov, m_nearPlane, m_farPlane, m_aspectRatio);
			}

			void ActivateShake() {
				Activate = true;
			}

			BodyType GetType() override {
				return BodyType::Camera;
			}

		private:
			float m_fov;
			float m_nearPlane;
			float m_farPlane;
			float m_aspectRatio;
		};
	}
}

#endif //EAE6320_ENTITYAI_CAMERABODYENTITY_H
