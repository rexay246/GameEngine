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
			void MoveRandomly();
			bool MoveTo(Math::sVector position);
			void MoveTo(float x, float y, float z);

		private:
			Math::sVector CurTargetLocation;
			cBoundingBox* BoundingBox;
			float AcceptanceRadius = 1.0f;
			bool Arrived = true;
		};
	}
}

#endif // EAE6320_ENTITYAI_CENTITYAI_H