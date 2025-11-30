#pragma once
#ifndef EAE6320_ENTITYAI_DEATHBODYENTITY_H
#define EAE6320_ENTITYAI_DEATHBODYENTITY_H

#include "BodyEntity.h"

namespace eae6320 {
	namespace BodyEntity {
		class cDeathBodyEntity : public cBodyEntity {
		public:
			BodyEntity::BodyType type = BodyEntity::Death;

			cDeathBodyEntity(Math::sVector position, float speed, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) :
				cBodyEntity(position, speed, _body, mesh_, effect_) {
			}

			cDeathBodyEntity(std::string path, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) :
				cBodyEntity(path, _body, mesh_, effect_) {
				entity->body = body;
			}

			BodyType GetType() override {
				return BodyType::Death;
			}
		};
	}
}

#endif //EAE6320_ENTITYAI_DEATHBODYENTITY_H
