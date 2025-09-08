#pragma once

#ifndef EAE6320_GRAPHICS_CVIEW_H
#define EAE6320_GRAPHICS_CVIEW_H

#include "Configuration.h"
#include "Graphics.h"

#ifdef EAE6320_PLATFORM_D3D
#include "Direct3D/Includes.h"
#else
#include "OpenGL/Includes.h"
#endif

#include <cstdint>
#include <Engine/Results/Results.h>
#include <Engine/Asserts/Asserts.h>

namespace eae6320 {
	namespace Graphics {
		class cView {
		public:
			void ClearViewBuffers(float cleanColor[3]);
			void SwapViewBuffers();
			cResult Initialize(const sInitializationParameters& i_initializationParameters);
			cResult CleanUp();

		private:
#ifdef EAE6320_PLATFORM_D3D
			// In Direct3D "views" are objects that allow a texture to be used a particular way:
	// A render target view allows a texture to have color rendered to it
			ID3D11RenderTargetView* s_renderTargetView = nullptr;
			// A depth/stencil view allows a texture to have depth rendered to it
			ID3D11DepthStencilView* s_depthStencilView = nullptr;
#endif
		};
	}
}


#endif // EAE6320_GRAPHICS_CVIEW_H
