#pragma once
#ifndef EAE6320_ENTITYAI_BALLBODYENTITY_H
#define EAE6320_ENTITYAI_BALLBODYENTITY_H

#include "BodyEntity.h"

namespace eae6320 {
	namespace BodyEntity {
		class cBallBodyEntity : public cBodyEntity {
		public:
			cBallBodyEntity(Math::sVector position, float speed, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) : 
			cBodyEntity (position, speed, _body, mesh_, effect_) {}

			cBallBodyEntity(std::string path, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) : 
				cBodyEntity(path, _body, mesh_, effect_) {
				entity->body = body;
			}

			void Update(float i_elapsedSecondCount_sinceLastUpdate, Physics::cPhysicsWorld* world, int index, std::map<Physics::PhysicsBody2D*, cBodyEntity*> entityTracker) {
				cBodyEntity::Update(i_elapsedSecondCount_sinceLastUpdate, world, index, entityTracker);

				std::vector<Physics::PhysicsBody2D*> result;
				Math::sVector extra = { 0, 0, 0 };
				if (entity->GetVelocity() != Math::sVector{ 0, 0, 0 }) {
					extra = entity->GetVelocity().GetNormalized();
				}
				if (world->OverlapBox(Util::ToVec2(entity->GetPosition() + extra * 0.5f), body->Width, body->Height, result))
				{
					for (auto collide : result) {
						if (collide != body) {
							Collide(entityTracker[collide]);
							entityTracker[collide]->Collide(this);
						}
					}
				}
				entity->MoveRandomlyBouncing(i_elapsedSecondCount_sinceLastUpdate);
			}

			void Collide(cBodyEntity* collider) override {
				OutputDebugStringW(L"Bouncing\n");
				entity->Bounce(collider->body);
			}
		};
	}
}

#endif //EAE6320_ENTITYAI_BALLBODYENTITY_H
