#pragma once
#ifndef EAE6320_ENTITYAI_CBOUNDINGBOX_H
#define EAE6320_ENTITYAI_CBOUNDINGBOX_H

#include "Engine/Graphics/Graphics.h"

namespace eae6320 {
	namespace EntityAI {
		class cBoundingBox {
		public:
			cBoundingBox(float loc_x, float loc_y, float loc_z, 
				float size_x, float size_y, float size_z);
			cBoundingBox(Math::sVector location, Math::sVector size);

			bool isValidPointInBoundingBox(Math::sVector location);
			Math::sVector getRandomPointInBoundingBox();

			float getMaxXRange();
			float getMinXRange();
			float getMaxYRange();
			float getMinYRange();
			float getMaxZRange();
			float getMinZRange();

		private:
			Math::sVector location = { 0, 0, 0 };
			Math::sVector sizes = { 1, 1, 1 };
		};
	}
}


#endif // EAE6320_ENTITYAI_CBOUNDINGBOX_H
