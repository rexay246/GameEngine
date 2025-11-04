#pragma once
#ifndef EAE6320_ENTITYAI_CENTITYAI_H
#define EAE6320_ENTITYAI_CENTITYAI_H

#include "cEntity.h"
#include "cBoundingBox.h"

namespace eae6320 {
	namespace EntityAI {
		class cEntityAI : public cEntity {
		public:
			cEntityAI(Math::sVector position, float speed, cBoundingBox* boundingBox = nullptr,
				float acceptanceRadius = 1.f);
			void SetPatrolPoints(Math::sVector* patrolPoints, int numOfPoints);
			void SetDetectionRange(float range);

			bool IsNearPosition(Math::sVector position);

			void MoveRandomly();
			bool MoveTo(Math::sVector position);
			void MoveTo(float x, float y, float z);
			void Patrol();
			void ChaseOrPatrol(Math::sVector position);

		private:
			Math::sVector CurTargetLocation;
			cBoundingBox* BoundingBox;
			float AcceptanceRadius = 1.0f;
			bool Arrived = true;

			Math::sVector* PatrolPoints = nullptr;
			int NumberOfPatrolPoints = 0;
			int CurrentPatrolIndex = -1;
			float DetectionRange = 0.f;

			float RunSpeed = 0.f;
			float WalkSpeed = 0.f;
		};
	}
}

#endif // EAE6320_ENTITYAI_CENTITYAI_H