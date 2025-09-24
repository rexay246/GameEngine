#pragma once
#ifndef EAE6320_GAMEOBJECT_CENTITY_H
#define EAE6320_GAMEOBJECT_CENTITY_H

#include "cGameObject.h"
#include "Engine/Graphics/Graphics.h"

namespace eae6320 {
	namespace GameObject {
		class cEntity : public cGameObject {
		public:

			void Initialize(Math::sVector position, float speed);
			void Rendering(Graphics::cMesh* mesh, Graphics::cEffect* effect, 
				const float i_elapsedSecondCount_sinceLastSimulationUpdate);

			float GetSpeed() { return m_speed; }

		private:
			float m_speed;
		};
	}
}

#endif // EAE6320_GAMEOBJECT_CENTITY_H