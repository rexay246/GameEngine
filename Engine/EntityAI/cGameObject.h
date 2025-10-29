#pragma once
#ifndef EAE6320_ENTITYAI_CGAMEOBJECT_H
#define EAE6320_ENTITYAI_CGAMEOBJECT_H

#include "Engine\Physics\sRigidBodyState.h"

namespace eae6320 {
	namespace EntityAI {
		class cGameObject {
		public:
			~cGameObject();
			Physics::sRigidBodyState* GetPhysicsState();
			void Update(const float i_elapsedSecondCount_sinceLastSimulationUpdate);

			void SetPosition(Math::sVector position);
			Math::sVector GetPosition() { return m_physicsState->position; }

			void SetVelocity(Math::sVector velocity);

			virtual void Rendering(const float i_elapsedSecondCount_sinceLastSimulationUpdate) = 0;

			float GetSpeed() { return m_speed; }
			void SetSpeed(float speed) { m_speed = speed; }

		private:
			Physics::sRigidBodyState* m_physicsState = new Physics::sRigidBodyState();
			float m_speed;
		};
	};
}

#endif // EAE6320_ENTITYAI_CGAMEOBJECT_H