#pragma once

#ifndef EAE6320_GRAPHICS_CEFFECT_H
#define EAE6320_GRAPHICS_CEFFECT_H

#include "Configuration.h"
#include "cShader.h"
#include "cRenderState.h"

#ifdef EAE6320_PLATFORM_GL
#include "OpenGL/Includes.h"
#endif

#include <cstdint>
#include <Engine/Results/Results.h>

namespace eae6320 {
	namespace Graphics {
		class cEffect {
		public:
			void BindEffect();
			cResult Initialize();
			cResult CleanUp();

			~cEffect();

		private:
			eae6320::Graphics::cShader* m_vertexShader = nullptr;
			eae6320::Graphics::cShader* m_fragmentShader = nullptr;
#ifdef EAE6320_PLATFORM_GL
			GLuint m_programId = 0;
#endif
			eae6320::Graphics::cRenderState m_renderState;

		private:
			cResult Initialize_Extra();
			cResult CleanUp_Extra();
			void BindEffect_PlatformSpecific();
		};
	}
}

#endif	// EAE6320_GRAPHICS_CEFFECT_H