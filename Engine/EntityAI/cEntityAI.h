/*
	This file provides more advanced assert functionality than the standard library
*/

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

			// Initialization
			static cResult Load(cEntityAI*& entityAI, const std::string& i_path);

			// Various Movement
			void MoveRandomly(float elapsedTime, Math::sVector* chaseTargetPosition = nullptr);
			void MoveRandomlyBouncing(float elapsedTime, Math::sVector* chaseTargetPosition = nullptr);
			bool MoveInOneDirection(Math::sVector vector, float elapsedTime, 
				Math::sVector* chaseTargetPosition = nullptr);
			bool MoveTo(Math::sVector position, float elapsedTime, Math::sVector* chaseTargetPosition = nullptr);
			void Patrol(float elapsedTime, Math::sVector* chaseTargetPosition = nullptr);
			void Chase(Math::sVector* chaseTargetPosition, float elapsedTime);
			void Idle();

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEntityAI);
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

		private:

			Math::sVector CurTargetLocation;
			cBoundingBox* BoundingBox;
			float AcceptanceRadius = 1.0f;

			Math::sVector* PatrolPoints = nullptr;
			unsigned int NumberOfPatrolPoints = 0;
			unsigned int CurrentPatrolIndex = -1;

			bool ChaseActive = false;
			float DetectionRange = 0.f;

			// Movement Speed
			float RunSpeed = 0.f;
			float WalkSpeed = 0.f;

			// Wait Times for chasing and patrolling
			float ChaseWaitTime = 0.f;
			float PatrolWaitTime = 0.f;
			// How long to wait after a chase
			float MaxChaseWaitTime = 1.f;
			// How long to wait in each patrol point
			float MaxPatrolWaitTime = 1.f;

			EnemyStates CurrentState = EnemyStates::Idle;
			EnemyMovementType CurrentMovementType = 
				EnemyMovementType::None;

		public:

			// Setters
			void SetPatrolPoints(Math::sVector* patrolPoints, int numOfPoints);
			void SetDetectionRange(float range) { DetectionRange = range; };
			void SetMaxChaseWaitTime(float waitTime) { MaxChaseWaitTime = waitTime; };
			void SetMaxPatrolWaitTime(float waitTime) { MaxPatrolWaitTime = waitTime; };
			void SetActiveChase(bool active) { ChaseActive = active; };
			void SetBoundingBox(cBoundingBox* bb) { BoundingBox = bb; };
			void SetStartingPatrolIndex(unsigned int index);

			// Getters
			float GetMaxChaseWaitTime() { return MaxChaseWaitTime; };
			float GetMaxPatrolWaitTime() { return MaxPatrolWaitTime; };
			float GetDetectionRange() { return DetectionRange; };
			bool GetActiveChase() { return ChaseActive; };

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

			void Move(Math::sVector vector);
			void FindClosestPatrolRoute();
			bool IsNearPosition(Math::sVector position);
		};
	}
}

#endif // EAE6320_ENTITYAI_CENTITYAI_H