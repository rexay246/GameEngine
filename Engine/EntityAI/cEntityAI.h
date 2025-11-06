#pragma once
#ifndef EAE6320_ENTITYAI_CENTITYAI_H
#define EAE6320_ENTITYAI_CENTITYAI_H

#include "cEntity.h"
#include "cBoundingBox.h"

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
			cEntityAI(Math::sVector position, float speed, cBoundingBox* boundingBox = nullptr,
				float acceptanceRadius = 1.f);
			void SetPatrolPoints(Math::sVector* patrolPoints, int numOfPoints);
			void SetDetectionRange(float range);

			bool IsNearPosition(Math::sVector position);

			void Move(Math::sVector vector);
			void MoveRandomly();
			void MoveRandomlyBouncing();
			bool MoveTo(Math::sVector position);
			void MoveTo(float x, float y, float z);
			void Idle();

			void Patrol();
			void FindClosestPatrolRoute();
			void Chase(Math::sVector position);
			void ChaseOrPatrol(Math::sVector position, float elapsedTime);

		private:
			Math::sVector CurTargetLocation;
			cBoundingBox* BoundingBox;
			float AcceptanceRadius = 1.0f;

			Math::sVector* PatrolPoints = nullptr;
			int NumberOfPatrolPoints = 0;
			int CurrentPatrolIndex = -1;
			float DetectionRange = 0.f;

			float RunSpeed = 0.f;
			float WalkSpeed = 0.f;

			float WaitTime = 0.f;
			float MaxWaitTime = 1.f;

			EnemyStates CurrentState = EnemyStates::Idle;
			EnemyMovementType CurrentMovementType = EnemyMovementType::None;
		};
	}
}

#endif // EAE6320_ENTITYAI_CENTITYAI_H