#pragma once
#ifndef EAE6320_ENTITYAI_BODYENTITY_H
#define EAE6320_ENTITYAI_BODYENTITY_H

#include "cEntityAI.h"
#include "Engine\Physics\cPhysicsBody2D.h"
#include <Engine/Platform/Platform.h>
#include <Engine/Graphics/cMesh.h>
#include <Engine/Graphics/cEffect.h>
#include <Engine/Physics/PhysicsUtil.h>

namespace eae6320 {
	namespace BodyEntity {
		enum BodyType {
			Default,
			Player,
			Wall,
			Ball,
			Alien,
			Death,
			Camera,
		};
	}
}

namespace eae6320 {
	namespace BodyEntity {
		class cBodyEntity {
		public:
			Physics::PhysicsBody2D* body;
			EntityAI::cEntityAI* entity = nullptr;
			Graphics::cMesh* mesh;
			Graphics::cEffect* effect;
			bool isDead = false;
			BodyEntity::BodyType type = BodyEntity::Default;

			cBodyEntity(Math::sVector position, float speed, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) {
				EntityAI::cEntityAI::Load(entity, position, speed);
				Initialize(_body, mesh_, effect_);
			}

			cBodyEntity(std::string path, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) {
				EntityAI::cEntityAI::Load(entity, path);
				Initialize(_body, mesh_, effect_);
			}

			void Initialize(Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) {
				if (_body != nullptr) {
					body = _body;
					body->MoveTo(Util::ToVec2(entity->GetPosition()));
				}
				if (mesh_ != nullptr && effect_ != nullptr)
					entity->setMeshAndEffect(mesh_, effect_);
			}

			void Update(float i_elapsedSecondCount_sinceLastUpdate, Physics::cPhysicsWorld* world, int index, std::map<Physics::PhysicsBody2D*, cBodyEntity*> entityTracker) {
				if (entity != nullptr) {
					entity->Update(i_elapsedSecondCount_sinceLastUpdate);
					body->MoveTo(Util::ToVec2(entity->GetPosition()));
				}
			}

			virtual void Collide(cBodyEntity* collider) {
				//OutputDebugStringW(L"Colliding\n");
			};

			virtual void Rendering(float i_elapsedSecondCount_sinceLastUpdate) {
				entity->Rendering(i_elapsedSecondCount_sinceLastUpdate);
			}

			void CleanUp() {
				if (entity != nullptr) {
					entity->CleanUp();
				}
			}

			virtual void MoveTo(Math::sVector position) {
				entity->SetPosition(position);
				body->MoveTo(Util::ToVec2(position));
			}

			virtual BodyType GetType() {
				return BodyType::Default;
			}
		};
	}
}

#endif //EAE6320_ENTITYAI_ALIENENTITY_H