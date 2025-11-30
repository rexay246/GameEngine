#pragma once
#ifndef EAE6320_ENTITYAI_PLAYERBODYENTITY_H
#define EAE6320_ENTITYAI_PLAYERBODYENTITY_H

#include "BodyEntity.h"

namespace eae6320 {
	namespace BodyEntity {
		class cPlayerBodyEntity : public cBodyEntity {
		public:
			BodyEntity::BodyType type = BodyEntity::Player;

			cPlayerBodyEntity(Math::sVector position, float speed, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) :
				cBodyEntity(position, speed, _body, mesh_, effect_) {
			}

			cPlayerBodyEntity(std::string path, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) :
				cBodyEntity(path, _body, mesh_, effect_) {
				entity->body = body;
			}

			void Update(float i_elapsedSecondCount_sinceLastUpdate, Physics::cPhysicsWorld* world, int index, std::map<Physics::PhysicsBody2D*, cBodyEntity*> entityTracker) {
				cBodyEntity::Update(i_elapsedSecondCount_sinceLastUpdate, world, index, entityTracker);
			}

			void MovePlayer(Math::sVector movement, Physics::cPhysicsWorld* world, int index, std::map<Physics::PhysicsBody2D*, cBodyEntity*> entityTracker) {
				std::vector<Physics::PhysicsBody2D*> result;
				if (movement != Math::sVector{ 0, 0, 0 }) {
					world->OverlapBox(Util::ToVec2(entity->GetPosition() + movement.GetNormalized() * 0.5f), body->Width, body->Height, result);
					bool collided = false;
					for (auto collide : result)
					{
						if (collide != body) {
							switch (entityTracker[collide]->GetType()) {
							case BodyType::Wall:
								entity->SetVelocity({ 0, 0, 0 });
								collided = true;
								break;
							case BodyType::Alien:
								break;
							case BodyType::Ball:
								//entityTracker[collide]->Collide(this);
								break;
							}
						}
					}
					if (collided)
						return;
					entity->SetVelocity(movement);
				}
				else {
					entity->SetVelocity({ 0, 0, 0 });
				}
			}

			BodyType GetType() override {
				return BodyType::Player;
			}
		};
	}
}

#endif //EAE6320_ENTITYAI_PLAYERBODYENTITY_H
