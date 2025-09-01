#pragma once

#ifndef EAE6320_GRAPHICS_CGEOMETRY_H
#define EAE6320_GRAPHICS_CGEOMETRY_H

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>

#ifdef EAE6320_PLATFORM_D3D
#include "Direct3D/Includes.h"
#include "cVertexFormat.h"
#else
#include "OpenGL/Includes.h"
#endif

// Forward Declarations
//=====================

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11Buffer;
#endif

namespace eae6320 {
	namespace Graphics {
		class cGeometry {

		public:
			void DrawGeometry();
			cResult Initialize();
			cResult CleanUp();

			cGeometry();
			~cGeometry();

		private:	

#ifdef EAE6320_PLATFORM_D3D
			eae6320::Graphics::cVertexFormat* s_vertexFormat = nullptr;
			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* s_vertexBuffer = nullptr;
#else
			// A vertex buffer holds the data for each vertex
			GLuint s_vertexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint s_vertexArrayId = 0;
#endif

		//private:
		//	cResult Initialize_platformSpecific();
		//	void DrawGeometry_platformSpecific();
		};
	}
}

#endif	// EAE6320_GRAPHICS_CGEOMETRY_H
