#pragma once
#ifndef EAE6320_CMESHBUILDER_H
#define EAE6320_CMESHBUILDER_H

#include <Tools/AssetBuildLibrary/iBuilder.h>
#include <External/Lua/Includes.h>
#include <Engine/Graphics/VertexFormats.h>
#include <fstream> 

#define MAX_VERTEX_COUNT 50000

namespace eae6320 {
	namespace Assets {
		class cMeshBuilder final : public iBuilder {
		private:

			// Build
			//------

			cResult Build(const std::vector<std::string>& i_arguments) final;
			cResult Load(const std::string& i_path);
			cResult LoadTableValues(lua_State& io_luaState);
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

			// Implementation
			//===============
		};
	}
}

#endif // EAE6320_CMESHBUILDER_H