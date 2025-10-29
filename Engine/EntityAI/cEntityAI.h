#pragma once
#ifndef EAE6320_ENTITYAI_CENTITYAI_H
#define EAE6320_ENTITYAI_CENTITYAI_H

#include "cEntity.h"

namespace eae6320 {
	namespace EntityAI {
		class cEntityAI : public cEntity {
		public:
			void MoveTo(Math::sVector position) {
				if (CurTargetLocation != position)				
					CurTargetLocation = position;
				Math::sVector currentPos = GetPosition();
				Math::sVector movementVector = CurTargetLocation - currentPos;
				movementVector.GetNormalized();
				SetVelocity(movementVector);
			}

			void MoveTo(float x, float y, float z) {
				MoveTo(Math::sVector(x, y, z));
			}
		private:
			Math::sVector CurTargetLocation;
		};
	}
}

#endif // EAE6320_ENTITYAI_CENTITYAI_H