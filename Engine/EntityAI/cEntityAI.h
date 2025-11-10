#pragma once
#ifndef EAE6320_ENTITYAI_CENTITYAI_H
#define EAE6320_ENTITYAI_CENTITYAI_H

#include "cEntity.h"
#include "cBoundingBox.h"
#include <Engine/Assets/ReferenceCountedAssets.h>
#include <External/Lua/Includes.h>

namespace eae6320 {
	namespace EntityAI {
		enum class EnemyStates {
			Idle,
			Moving
		};

		enum class EnemyMovementType {
			Chase,
			Patrol,
			Random,
			None
		};
	}
}

namespace eae6320 {
	namespace EntityAI {
		class cEntityAI : public cEntity {
		public:
			static cResult Load(cEntityAI*& entityAI, const std::string& i_path);
			void SetPatrolPoints(Math::sVector* patrolPoints, int numOfPoints);
			void SetDetectionRange(float range);
			void SetMaxChaseWaitTime(float waitTime);
			void SetMaxPatrolWaitTime(float waitTime);
			void SetActiveChase(bool active);

			float GetMaxChaseWaitTime() { return MaxChaseWaitTime; };
			float GetMaxPatrolWaitTime() { return MaxPatrolWaitTime; };
			bool GetActiveChase() { return ChaseActive; };

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEntityAI);
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			void MoveRandomly(float elapsedTime, Math::sVector* chaseTargetPosition = nullptr);
			void MoveRandomlyBouncing(float elapsedTime, Math::sVector* chaseTargetPosition = nullptr);
			bool MoveInOneDirection(Math::sVector vector, float elapsedTime, Math::sVector* chaseTargetPosition = nullptr);
			bool MoveTo(Math::sVector position, float elapsedTime, Math::sVector* chaseTargetPosition = nullptr);
			void Patrol(float elapsedTime, Math::sVector* chaseTargetPosition = nullptr);
			void Chase(Math::sVector* chaseTargetPosition, float elapsedTime);
			void Idle();

		private:
			static cResult Initialize(cEntityAI*& entityAI, Math::sVector position,
				float WalkSpeed, float RunSpeed, cBoundingBox* boundingBox = nullptr,
				float acceptanceRadius = 1.f, Math::sVector* patrolPoints = nullptr, int numOfPoints = 0, 
				float detectionRange = 0, bool activeChase = false, float maxPatrolWaitTime = 0.f,
				float maxChaseWaitTime = 0.f);
			cEntityAI(Math::sVector position, float WalkSpeed, float RunSpeed,
				cBoundingBox* boundingBox, float acceptanceRadius,
				Math::sVector* patrolPoints, int numOfPoints,
				float detectionRange, bool activeChase, float maxPatrolWaitTime,
				float maxChaseWaitTime);
			~cEntityAI();
			void Move(Math::sVector vector, float elapsedTime);
			void FindClosestPatrolRoute();
			bool IsNearPosition(Math::sVector position);

			Math::sVector CurTargetLocation;
			cBoundingBox* BoundingBox;
			float AcceptanceRadius = 1.0f;

			Math::sVector* PatrolPoints = nullptr;
			unsigned int NumberOfPatrolPoints = 0;
			unsigned int CurrentPatrolIndex = -1;

			bool ChaseActive = false;
			float DetectionRange = 0.f;

			float RunSpeed = 0.f;
			float WalkSpeed = 0.f;

			float ChaseWaitTime = 0.f;
			float PatrolWaitTime = 0.f;
			float MaxChaseWaitTime = 1.f;
			float MaxPatrolWaitTime = 1.f;

			EnemyStates CurrentState = EnemyStates::Idle;
			EnemyMovementType CurrentMovementType = EnemyMovementType::None;

		private:
			cResult LoadTableValues(cEntityAI*& entityAI, lua_State& io_luaState);
			cResult LoadTableValues_Position(lua_State& io_luaState, Math::sVector& pos);
			cResult LoadTableValues_Speed(lua_State& io_luaState, float& walkSpeed, float& runSpeed);
			cResult LoadTableValues_BoundingBox(lua_State& io_luaState, Math::sVector& bbPos, Math::sVector& bbLen);
			cResult LoadTableValues_BBPosition(lua_State& io_luaState, Math::sVector& bbPos);
			cResult LoadTableValues_BBLength(lua_State& io_luaState, Math::sVector& bbLen);
			cResult LoadTableValues_AcceptanceRadius(lua_State& io_luaState, float& acceptanceRad);
			cResult LoadTableValues_PatrolPoints(lua_State& io_luaState, Math::sVector*& patrolPoints, unsigned int& num, float& time);
			cResult LoadTableValues_PatrolPoints_Values(lua_State& io_luaState, Math::sVector*& patrolPoints, unsigned int& num);
			cResult LoadTableValues_Chase(lua_State& io_luaState, float& detectionRange, bool& activeChase, float& maxWaitTime);
		};
	}
}

#endif // EAE6320_ENTITYAI_CENTITYAI_H