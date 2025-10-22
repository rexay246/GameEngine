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
#include <string>
#include <External/Lua/Includes.h>

#define MAX_VERTEX_COUNT 50000

// Forward Declarations
//=====================

#ifdef EAE6320_PLATFORM_D3D
struct ID3D11Buffer;
#endif

namespace eae6320 {
	namespace Graphics {
		class cMesh {

		public:
			static cResult LoadFromBinary(cMesh*& o_mesh, const std::string& i_path);
			static cResult Load(cMesh*& o_mesh, const std::string& i_path);
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
				unsigned int vertexCount,
				uint16_t* indexData,
				unsigned int indexCount);
			static cResult CreateMesh(cMesh*& o_mesh,
				eae6320::Graphics::VertexFormats::sVertex_mesh* vertexData,
				unsigned int vertexCount,
				uint16_t* indexData,
				unsigned int indexCount);

			cResult LoadTableValues(lua_State& io_luaState, cMesh*& o_mesh);
			cResult LoadTableValues_Vertices(lua_State& io_luaState, 
				eae6320::Graphics::VertexFormats::sVertex_mesh*& vertexData,
				unsigned int& vertexCount);
			cResult LoadTableValues_Vertices_Paths(lua_State& io_luaState,
				eae6320::Graphics::VertexFormats::sVertex_mesh*& vertexData,
				unsigned int& vertexCount);
			cResult LoadTableValues_Indices(lua_State& io_luaState, 
				uint16_t*& indexData, unsigned int& indexCount);
			cResult LoadTableValues_Indices_Paths(lua_State& io_luaState, 
				uint16_t*& indexData, unsigned int& indexCount);
			cResult LoadTableValues_Indices_Paths_Vertices(lua_State& io_luaState,
				uint16_t*& indexData, unsigned int& indexCount, int startCount,
				int indexCountPerMesh);

			cResult CleanUp();
			cMesh() = default;
			~cMesh();
		};
	}
}

#endif	// EAE6320_GRAPHICS_CMESH_H
