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
			void SetMaxWaitTime(float waitTime);
			void SetActiveChase(bool active);
			void SetChaseTarget(Math::sVector* chaseTargetPosition);

			float GetMaxWaitTime() { return MaxWaitTime; };
			bool GetActiveChase() { return ChaseActive; };
			Math::sVector GetChaseTargetLocation() { return *chaseTarget; };

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEntityAI);
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

			bool IsNearPosition(Math::sVector position);

			void Move(Math::sVector vector, float elapsedTime);
			void MoveRandomly(float elapsedTime);
			void MoveRandomlyBouncing(float elapsedTime);
			bool MoveTo(Math::sVector position, float elapsedTime);
			void MoveTo(float x, float y, float z, float elapsedTime);
			void Idle();

			void Patrol(float elapsedTime);
			void FindClosestPatrolRoute();
			void Chase(Math::sVector position, float elapsedTime);
			void ChaseOrPatrol(Math::sVector position, float elapsedTime);

		private:
			static cResult Initialize(cEntityAI*& entityAI, Math::sVector position,
				float WalkSpeed, float RunSpeed, cBoundingBox* boundingBox = nullptr,
				float acceptanceRadius = 1.f, Math::sVector* patrolPoints = nullptr, int numOfPoints = 0, 
				float detectionRange = 0, bool activeChase = false, float maxWaitTime = 0.f);
			cEntityAI(Math::sVector position, float WalkSpeed, float RunSpeed,
				cBoundingBox* boundingBox, float acceptanceRadius,
				Math::sVector* patrolPoints, int numOfPoints,
				float detectionRange, bool activeChase, float maxWaitTime);
			~cEntityAI();

			Math::sVector CurTargetLocation;
			cBoundingBox* BoundingBox;
			float AcceptanceRadius = 1.0f;

			Math::sVector* PatrolPoints = nullptr;
			unsigned int NumberOfPatrolPoints = 0;
			unsigned int CurrentPatrolIndex = -1;

			bool ChaseActive = false;
			float DetectionRange = 0.f;
			Math::sVector* chaseTarget = nullptr;

			float RunSpeed = 0.f;
			float WalkSpeed = 0.f;

			float WaitTime = 0.f;
			float MaxWaitTime = 1.f;

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
			cResult LoadTableValues_PatrolPoints(lua_State& io_luaState, Math::sVector*& patrolPoints, unsigned int& num);
			cResult LoadTableValues_PatrolPoints_Values(lua_State& io_luaState, Math::sVector*& patrolPoints, unsigned int& num);
			cResult LoadTableValues_Chase(lua_State& io_luaState, float& detectionRange, bool& activeChase, float& maxWaitTime);
		};
	}
}

#endif // EAE6320_ENTITYAI_CENTITYAI_H