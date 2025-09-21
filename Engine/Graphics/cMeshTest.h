#pragma once

#ifndef EAE6320_GRAPHICS_CMESH_H
#define EAE6320_GRAPHICS_CMESH_H

#include "Configuration.h"
#include "cShader.h"
#include "cRenderState.h"

#ifdef EAE6320_PLATFORM_GL
#include "OpenGL/Includes.h"
#endif

#include <cstdint>
#include <Engine/Results/Results.h>
#include <Engine/Assets/cbReferenceCountedAsset.h>

namespace eae6320 {
	namespace Graphics {
		class cEffect {
		public:
			static cResult CreateEffect(cEffect*& o_effect,
				std::string vertexShader,
				std::string fragmentShader,
				uint8_t u_renderStateBits = 0);
			void BindEffect();

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cEffect);
			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

		private:
			eae6320::Graphics::cShader* m_vertexShader = nullptr;
			eae6320::Graphics::cShader* m_fragmentShader = nullptr;
#ifdef EAE6320_PLATFORM_GL
			GLuint m_programId = 0;
#endif
			eae6320::Graphics::cRenderState* m_renderState =
				new cRenderState();

		private:

			cResult Initialize_platformSpecificExtra();
			cResult CleanUp_platformSpecificExtra();
			void BindEffect_PlatformSpecific();

			cResult Initialize(std::string vertexShader,
				std::string fragmentShader,
				uint8_t u_renderStateBits);
			cResult CleanUp();

			cEffect() = default;
			~cEffect();
		};
	}
}

#endif	// EAE6320_GRAPHICS_CMESH_H
