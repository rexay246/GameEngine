#pragma once
#ifndef EAE6320_GAMEOBJECT_CENTITY_H
#define EAE6320_GAMEOBJECT_CENTITY_H

#include "cGameObject.h"
#include "Engine/Graphics/Graphics.h"

namespace eae6320 {
	namespace GameObject {
		class cEntity : public cGameObject {
		public:
			void Initialize(Graphics::VertexFormats::sVertex_mesh* vertexData, 
				int vertexCount, uint16_t* indexData, int indexCount,
				std::string vertexShader, std::string fragmentShader,
				Math::sVector position, float speed);
			void Rendering(const float i_elapsedSecondCount_sinceLastSimulationUpdate);
			void CleanUp();

			float GetSpeed() { return m_speed; }

		private:
			Graphics::cEffect* m_effect;
			Graphics::cMesh* m_mesh;
			float m_speed;
		};
	}
}

#endif // EAE6320_GAMEOBJECT_CENTITY_H