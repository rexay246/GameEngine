#pragma once
#ifndef EAE6320_ENTITYAI_BALLBODYENTITY_H
#define EAE6320_ENTITYAI_BALLBODYENTITY_H

#include "BodyEntity.h"
#include "CameraBodyEntity.h"

namespace eae6320 {
	namespace BodyEntity {
		class cBallBodyEntity : public cBodyEntity {
		public:
			BodyEntity::BodyType type = BodyEntity::Ball;
			float waitPeriodBeforeMoving = 1.0f;
			float respawnTime = 1.5f;
			cBodyEntity* player;
			float maxSpeed = 8.f;
			BodyEntity::cCameraBodyEntity* camera;

			float ClampFloat(float value, float max) {
				if (value > max)
					return max;
				return value;
			}

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
						entity->MoveTo(Math::sVector(player->entity->GetPosition().x, player->entity->GetPosition().y + 1.5f, 0), i_elapsedSecondCount_sinceLastUpdate);
					return;
				}
				if (isDead) {
					entity->Idle();
					isDead = false;
					entity->SetSpeed(3);
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
				if (!isDead) {
					entity->MoveRandomlyBouncing(i_elapsedSecondCount_sinceLastUpdate);
				}

			}

			void Collide(cBodyEntity* collider) override {
				if (isDead)
					return;
				switch (collider->GetType()) {
				case BodyType::Player:
					entity->SetSpeed(ClampFloat(entity->GetSpeed() + 0.2f, maxSpeed));
					if (std::fabs((collider->entity->GetPosition() - entity->GetPosition()).GetLength()) < 1.f) {
						entity->BounceWall(collider->body);
					}
					else {
						entity->Bounce(collider->body);
					}
					camera->ActivateShake();
					AudioSystem::PlaySFX("jump", 0.2f);
					break;
				case BodyType::Wall:
					entity->SetSpeed(ClampFloat(entity->GetSpeed() + 0.2f, maxSpeed));
					entity->BounceWall(collider->body);
					camera->ActivateShake();
					AudioSystem::PlaySFX("jump", 0.2f);
					break;
				case BodyType::Death:
					Die();
					break;
				default:
					entity->SetSpeed(ClampFloat(entity->GetSpeed() + 0.2f, maxSpeed));
					entity->Bounce(collider->body);
					camera->ActivateShake();
					AudioSystem::PlaySFX("jump", 0.2f);
					break;
				}
			}

			void Die() {
				if (isDead)
					return;
				isDead = true;
				waitPeriodBeforeMoving = respawnTime;
				entity->Idle();
				//MoveTo(Math::sVector(0, -2.0, 0)); 
				entity->SetSpeed(5);
			}

			BodyType GetType() override {
				return BodyType::Ball;
			}
		};
	}
}

#endif //EAE6320_ENTITYAI_BALLBODYENTITY_H
