#pragma once
#ifndef EAE6320_ENTITYAI_ALIENBODYENTITY_H
#define EAE6320_ENTITYAI_ALIENBODYENTITY_H

#include "BodyEntity.h"

namespace eae6320 {
	namespace BodyEntity {
		class cAlienBodyEntity : public cBodyEntity {
		public:
			BodyEntity::BodyType type = BodyEntity::Alien;
			Math::sVector originalLocation;
			float respawnTimer = 1.0f;
			float MaxRespawnTime = 2.0f;

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
				originalLocation = entity->GetPosition();
				isDead = false;
			}

			void Update(float i_elapsedSecondCount_sinceLastUpdate, Physics::cPhysicsWorld* world, int index, std::map<Physics::PhysicsBody2D*, cBodyEntity*> entityTracker) {
				if (isDead) {
					if (respawnTimer > 0.f) {
						respawnTimer -= i_elapsedSecondCount_sinceLastUpdate;			
					}
					else {
						Respawn();
					}
				}
				cBodyEntity::Update(i_elapsedSecondCount_sinceLastUpdate, world, index, entityTracker);

				//entity->Navigate(i_elapsedSecondCount_sinceLastUpdate);
				entity->MoveRandomlyInOneDirection(i_elapsedSecondCount_sinceLastUpdate, Math::sVector(0, -1, 0));
				//entity->MoveDistance(Math::sVector(0.3f, 0.f, 0.f), i_elapsedSecondCount_sinceLastUpdate);
			}

			void Collide(cBodyEntity* collider) override {
				//OutputDebugStringW(L"Bouncing\n");
				Die();
				//entity->Bounce(collider->body);
			}

			void Rendering(float i_elapsedSecondCount_sinceLastUpdate) override {
				if (!isDead)
					cBodyEntity::Rendering(i_elapsedSecondCount_sinceLastUpdate);
			}

			void Respawn() {
				isDead = false;
			}

			void Die() {
				isDead = true;
				respawnTimer = RandomFloat(0.5, MaxRespawnTime);
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
