#pragma once
#ifndef EAE6320_ENTITYAI_BALLBODYENTITY_H
#define EAE6320_ENTITYAI_BALLBODYENTITY_H

#include "BodyEntity.h"

namespace eae6320 {
	namespace BodyEntity {
		class cBallBodyEntity : public cBodyEntity {
		public:
			BodyEntity::BodyType type = BodyEntity::Ball;
			float waitPeriodBeforeMoving = 1.0f;
			float respawnTime = 1.5f;
			cBodyEntity* player;

			cBallBodyEntity(Math::sVector position, float speed, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) : 
			cBodyEntity (position, speed, _body, mesh_, effect_) {
				isDead = true;
			}

			cBallBodyEntity(std::string path, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) : 
				cBodyEntity(path, _body, mesh_, effect_) {
				entity->body = body;
				isDead = true;
			}

			void Update(float i_elapsedSecondCount_sinceLastUpdate, Physics::cPhysicsWorld* world, int index, std::map<Physics::PhysicsBody2D*, cBodyEntity*> entityTracker) {
				cBodyEntity::Update(i_elapsedSecondCount_sinceLastUpdate, world, index, entityTracker);

				if (waitPeriodBeforeMoving > 0.f) {
					waitPeriodBeforeMoving -= i_elapsedSecondCount_sinceLastUpdate;
					if (player)
						entity->MoveTo(Math::sVector(player->entity->GetPosition().x, entity->GetPosition().y, 0), i_elapsedSecondCount_sinceLastUpdate);
					return;
				}
				if (isDead) {
					entity->Idle();
					isDead = false;
				}

				std::vector<Physics::PhysicsBody2D*> result;
				Math::sVector extra = { 0, 0, 0 };
				if (entity->GetVelocity() != Math::sVector{ 0, 0, 0 }) {
					extra = entity->GetVelocity().GetNormalized();
				}
				if (world->OverlapBox(Util::ToVec2(entity->GetPosition() + extra * 0.5f), body->Width, body->Height, result))
				{
					for (auto collide : result) {
						if (collide != body && !entityTracker[collide]->isDead) {
							Collide(entityTracker[collide]);
							entityTracker[collide]->Collide(this);
						}
					}
				}
				if (!isDead)
					entity->MoveRandomlyBouncing(i_elapsedSecondCount_sinceLastUpdate);
			}

			void Collide(cBodyEntity* collider) override {
				if (isDead)
					return;
				switch (collider->GetType()) {
				case BodyType::Wall:
					entity->BounceWall(collider->body);
					break;
				case BodyType::Death:
					Die();
					break;
				default:
					entity->Bounce(collider->body);
					break;
				}
			}

			void Die() {
				isDead = true;
				waitPeriodBeforeMoving = respawnTime;
				entity->Idle();
				MoveTo(Math::sVector(0, -2, 0)); 
			}

			BodyType GetType() override {
				return BodyType::Ball;
			}
		};
	}
}

#endif //EAE6320_ENTITYAI_BALLBODYENTITY_H
