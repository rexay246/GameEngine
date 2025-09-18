#pragma once

#ifndef EAE6320_GRAPHICS_CMESH_H
#define EAE6320_GRAPHICS_CMESH_H

#include "Configuration.h"
#include "VertexFormats.h"

#ifdef EAE6320_PLATFORM_D3D
#include "Direct3D/Includes.h"
#include "cVertexFormat.h"
#else
#include "OpenGL/Includes.h"
#endif

#include <cstdint>
#include <Engine/Results/Results.h>
#include <Engine/Assets/ReferenceCountedAssets.h>

// Forward Declarations
//=====================

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11Buffer;
#endif

namespace eae6320 {
	namespace Graphics {
		class cMesh {

		public:
			static cResult CreateMesh(cMesh*& o_mesh,
				eae6320::Graphics::VertexFormats::sVertex_mesh* vertexData,
				int vertexCount,
				uint16_t* indexData,
				int indexCount);
			void DrawMesh();

			EAE6320_ASSETS_DECLAREREFERENCECOUNTINGFUNCTIONS();
			EAE6320_ASSETS_DECLAREDELETEDREFERENCECOUNTEDFUNCTIONS(cMesh);
			//EAE6320_ASSETS_DECLAREREFERENCECOUNT();

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
			unsigned int m_indexCountToRender = 0;

		public:

			EAE6320_ASSETS_DECLAREREFERENCECOUNT();

		private:
			cResult Initialize(
				eae6320::Graphics::VertexFormats::sVertex_mesh* vertexData,
				int vertexCount,
				uint16_t* indexData,
				int indexCount);
			cResult CleanUp();
			cMesh() = default;
			~cMesh();
		};
	}
}

#endif	// EAE6320_GRAPHICS_CMESH_H
