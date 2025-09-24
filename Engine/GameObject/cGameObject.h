#pragma once
#ifndef EAE6320_GAMEOBJECT_CGAMEOBJECT_H
#define EAE6320_GAMEOBJECT_CGAMEOBJECT_H

#include "Engine\Physics\sRigidBodyState.h"

namespace eae6320 {
	namespace GameObject {
		class cGameObject {
		public:
			~cGameObject();
			Physics::sRigidBodyState* GetPhysicsState();
			void Update(const float i_elapsedSecondCount_sinceLastSimulationUpdate);

			void SetPosition(Math::sVector position);
			Math::sVector GetPosition() { return m_physicsState->position; }

			void SetVelocity(Math::sVector velocity);
		private:
			Physics::sRigidBodyState* m_physicsState = new Physics::sRigidBodyState();
		};
	};
}

#endif // EAE6320_GAMEOBJECT_CGAMEOBJECT_H