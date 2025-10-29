#pragma once
#ifndef EAE6320_ENTITYAI_CENTITYAI_H
#define EAE6320_ENTITYAI_CENTITYAI_H

#include "Engine\GameObject\cEntity.h"

namespace eae6320 {
	namespace EntityAI {
		class cEntityAI : public GameObject::cEntity {
		public:
			int checker() {
				return check;
			}
		private:
			int check = 5;
		};
	}
}

#endif // EAE6320_ENTITYAI_CENTITYAI_H