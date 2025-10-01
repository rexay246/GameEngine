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
			void setMeshAndEffect(Graphics::cMesh* mesh, Graphics::cEffect* effect);
			void Rendering(const float i_elapsedSecondCount_sinceLastSimulationUpdate) final;
			void CleanUp();

		private:
			Graphics::cMesh* m_mesh = nullptr;
			Graphics::cEffect* m_effect = nullptr;
		};
	}
}

#endif // EAE6320_GAMEOBJECT_CENTITY_H