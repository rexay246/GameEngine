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
		class cBodyEntity {
		public:
			Physics::PhysicsBody2D* body;
			EntityAI::cEntityAI* entity = nullptr;
			Graphics::cMesh* mesh;
			Graphics::cEffect* effect;

			cBodyEntity(Math::sVector position, float speed, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) {
				EntityAI::cEntityAI::Load(entity, position, speed);
				Initialize(_body, mesh_, effect_);
			}

			cBodyEntity(std::string path, Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) {
				EntityAI::cEntityAI::Load(entity, path);
				Initialize(_body, mesh_, effect_);
			}

			void Initialize(Physics::PhysicsBody2D* _body, Graphics::cMesh* mesh_, Graphics::cEffect* effect_) {
				body = _body;
				body->MoveTo(Util::ToVec2(entity->GetPosition()));
				entity->setMeshAndEffect(mesh_, effect_);
			}

			void Update(float i_elapsedSecondCount_sinceLastUpdate, Physics::cPhysicsWorld* world, int index, std::map<Physics::PhysicsBody2D*, cBodyEntity*> entityTracker) {
				if (entity != nullptr) {
					entity->Update(i_elapsedSecondCount_sinceLastUpdate);
					body->MoveTo(Util::ToVec2(entity->GetPosition()));
				}

				//std::vector<Physics::PhysicsBody2D*> result;
				//Math::sVector extra = { 0, 0, 0 };
				//if (entity->GetVelocity() != Math::sVector{0, 0, 0}) {
				//	extra = entity->GetVelocity().GetNormalized();
				//}
				//if (world->OverlapBox(Util::ToVec2(entity->GetPosition() + extra * 0.5f), body->Width, body->Height, result))
				//{
				//	for (auto collide : result) {
				//		if (collide != body) {
				//			entityTracker[collide]->Collide(this);
				//		}
				//	}
				//}
			}

			virtual void Collide(cBodyEntity* collider) {
				OutputDebugStringW(L"Checking\n");
			};

			void CleanUp() {
				if (entity != nullptr) {
					entity->CleanUp();
				}
			}
		};
	}
}

#endif //EAE6320_ENTITYAI_ALIENENTITY_H