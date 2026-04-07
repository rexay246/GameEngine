#pragma once
#ifndef EAE6320_ENTITYAI_ALIENBODYENTITY_H
#define EAE6320_ENTITYAI_ALIENBODYENTITY_H

#include "BodyEntity.h"
#include "PlayerBodyEntity.h"

namespace eae6320 {
	namespace BodyEntity {
		class cAlienBodyEntity : public cBodyEntity {
		public:
			BodyEntity::BodyType type = BodyEntity::Alien;
			float respawnTimer = 1.0f;
			float MaxRespawnTime = 2.0f;
			BodyEntity::cPlayerBodyEntity* breaker;

			float RandomFloat(float max, float min) {
				float random = static_cast<float>(::rand()) / static_cast<float>(RAND_MAX);
				//std::wstring msg = L"Random float: " + std::to_wstring(random) + L"\r\n";
				//OutputDebugStringW(msg.c_str());
				return min + random * (max - min);
			}

			cAlienBodyEntity(Math::sVector position, float speed, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) :
				cBodyEntity(position, speed, _body, mesh_, effect_) {
			}

			cAlienBodyEntity(std::string path, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) :
				cBodyEntity(path, _body, mesh_, effect_) {
				entity->body = body;
				isDead = false;
				respawnTimer = RandomFloat(0.5, MaxRespawnTime);
			}

			void Update(float i_elapsedSecondCount_sinceLastUpdate, Physics::cPhysicsWorld* world, int index, std::map<Physics::PhysicsBody2D*, cBodyEntity*> entityTracker) {
				if (isDead) {
					if (respawnTimer > 0.f) {
						respawnTimer -= i_elapsedSecondCount_sinceLastUpdate;
						return;
					}
					else {
						Respawn();
					}
				}
				cBodyEntity::Update(i_elapsedSecondCount_sinceLastUpdate, world, index, entityTracker);
				entity->MoveRandomlyInOneDirection(i_elapsedSecondCount_sinceLastUpdate, Math::sVector(0, -1, 0));

				std::vector<Physics::PhysicsBody2D*> result;
				if (world->OverlapBox(Util::ToVec2(entity->GetPosition()), body->Width + 2.f, body->Height + 2.f, result))
				{
					for (auto collide : result) {
						if (collide != body && !entityTracker[collide]->isDead) {
							if (entityTracker[collide]->GetType() == BodyType::Death)
								breaker->GameLost = true;
						}
					}
				}
			}

			void Collide(cBodyEntity* collider) override {
				//OutputDebugStringW(L"Bouncing\n");
				Die();
			}

			void Rendering(float i_elapsedSecondCount_sinceLastUpdate) override {
				if (!isDead)
					cBodyEntity::Rendering(i_elapsedSecondCount_sinceLastUpdate);
			}

			void Respawn() {
				respawnTimer = RandomFloat(0.5, MaxRespawnTime);
				isDead = false;
				entity->Idle();
			}

			void Die() {
				isDead = true;
				cBodyEntity::MoveTo(Math::sVector(RandomFloat(-2.0f, 2.0f), RandomFloat(5, 7), 0));
				entity->Idle();
				AudioSystem::PlaySFX("death", 0.3f);
			}

			void MoveTo(Math::sVector position) override {
				entity->SetPosition(position);
				body->MoveTo(Util::ToVec2(position));
				originalLocation = position;
			}

			BodyType GetType() override {
				return BodyType::Alien;
			}
		};
	}
}

#endif //EAE6320_ENTITYAI_ALIENBODYENTITY_H
