#pragma once

#ifndef EAE6320_GRAPHICS_CMESH_H
#define EAE6320_GRAPHICS_CMESH_H

#include "Configuration.h"

#ifdef EAE6320_PLATFORM_D3D
#include "Direct3D/Includes.h"
#include "cVertexFormat.h"
#else
#include "OpenGL/Includes.h"
#endif

#include <cstdint>
#include <Engine/Results/Results.h>

// Forward Declarations
//=====================

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11Buffer;
#endif

namespace eae6320 {
	namespace Graphics {
		class cMesh {

		public:
			void DrawMesh();
			cResult Initialize();
			cResult CleanUp();
			~cMesh();

		private:	

#ifdef EAE6320_PLATFORM_D3D
			eae6320::Graphics::cVertexFormat* m_vertexFormat = nullptr;
			// A vertex buffer holds the data for each vertex
			ID3D11Buffer* m_vertexBuffer = nullptr;
			ID3D11Buffer* m_indexBuffer = nullptr;
#else
			// A vertex buffer holds the data for each vertex
			GLuint m_vertexBufferId = 0;
			// A vertex array encapsulates the vertex data as well as the vertex input layout
			GLuint m_vertexArrayId = 0;
			GLuint m_indexBufferId = 0;
#endif
		};
	}
}

#endif	// EAE6320_GRAPHICS_CMESH_H
