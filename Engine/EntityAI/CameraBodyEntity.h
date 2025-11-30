#pragma once
#ifndef EAE6320_ENTITYAI_CAMERABODYENTITY_H
#define EAE6320_ENTITYAI_CAMERABODYENTITY_H

#include "BodyEntity.h"

namespace eae6320 {
	namespace BodyEntity {
		class cCameraBodyEntity : public cBodyEntity {
		public:
			BodyEntity::BodyType type = BodyEntity::Camera;

			cCameraBodyEntity(Math::sVector position, float speed, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) :
				cBodyEntity(position, speed, _body, mesh_, effect_) {
			}

			cCameraBodyEntity(std::string path, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) :
				cBodyEntity(path, _body, mesh_, effect_) {
				entity->body = body;
			}

			BodyType GetType() override {
				return BodyType::Camera;
			}
		};
	}
}

#endif //EAE6320_ENTITYAI_CAMERABODYENTITY_H
