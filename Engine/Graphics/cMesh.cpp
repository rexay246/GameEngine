#include "cMesh.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Platform/Platform.h>


eae6320::cResult eae6320::Graphics::cMesh::LoadFromBinary(cMesh*& o_mesh, const std::string& i_path)
{
	auto result = eae6320::Results::Success;
	Platform::sDataFromFile dataFromFile;
	if (!(result = Platform::LoadBinaryFile(i_path.c_str(), dataFromFile))) {
		Logging::OutputError("Error when reading from binary file");
		o_mesh = nullptr;
		return result;
	}
	auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
	const auto finalOffset = currentOffset + dataFromFile.size;

	uint16_t vertexCount;
	memcpy(&vertexCount, reinterpret_cast<void*>(currentOffset), sizeof(vertexCount));
	currentOffset += sizeof(vertexCount);

	uint16_t indexCount;
	memcpy(&indexCount, reinterpret_cast<void*>(currentOffset), sizeof(indexCount));
	currentOffset += sizeof(indexCount);

	auto* const vertexArray = reinterpret_cast<VertexFormats::sVertex_mesh*>(currentOffset);
	currentOffset += sizeof(VertexFormats::sVertex_mesh) * vertexCount;

	auto* const indexArray = reinterpret_cast<uint16_t*>(currentOffset);
	currentOffset += sizeof(uint16_t) * indexCount;

	result = CreateMesh(o_mesh, vertexArray, vertexCount, indexArray, indexCount);

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::Load(cMesh*& o_mesh, const std::string& i_path)
{
	auto result = eae6320::Results::Success;
	const char* path = i_path.c_str();
	lua_State* luaState = nullptr;
	eae6320::cScopeGuard scopeGuard_onExit([&luaState]
		{
			if (luaState)
			{
				// If I haven't made any mistakes
				// there shouldn't be anything on the stack
				// regardless of any errors
				EAE6320_ASSERT(lua_gettop(luaState) == 0);

				lua_close(luaState);
				luaState = nullptr;
			}
		});
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			Logging::OutputError("Failed to create a new Lua state");
			return result;
		}
	}
	// Load the asset file as a "chunk",
	// meaning there will be a callable function at the top of the stack
	const auto stackTopBeforeLoad = lua_gettop(luaState);
	{
		const auto luaResult = luaL_loadfile(luaState, path);
		if (luaResult != LUA_OK)
		{
			result = eae6320::Results::Failure;
			Logging::OutputError(lua_tostring(luaState, -1));
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}
	// Execute the "chunk", which should load the asset
	// into a table at the top of the stack
	{
		constexpr int argumentCount = 0;
		constexpr int returnValueCount = LUA_MULTRET;	// Return _everything_ that the file returns
		constexpr int noMessageHandler = 0;
		const auto luaResult = lua_pcall(luaState, argumentCount, returnValueCount, noMessageHandler);
		if (luaResult == LUA_OK)
		{
			// A well-behaved asset file will only return a single value
			const auto returnedValueCount = lua_gettop(luaState) - stackTopBeforeLoad;
			if (returnedValueCount == 1)
			{
				// A correct asset file _must_ return a table
				if (!lua_istable(luaState, -1))
				{
					result = eae6320::Results::InvalidFile;
					Logging::OutputError("Asset files must return a table (instead of a ",
						luaL_typename(luaState, -1), ")");
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				Logging::OutputError("Asset files must return a single table (instead of ",
					returnedValueCount, " values)");
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			Logging::OutputError(lua_tostring(luaState, -1));
			// Pop the error message
			lua_pop(luaState, 1);
			return result;
		}
	}

	// If this code is reached the asset file was loaded successfully,
	// and its table is now at index -1
	eae6320::cScopeGuard scopeGuard_popAssetTable([luaState]
		{
			lua_pop(luaState, 1);
		});

	result = o_mesh->LoadTableValues(*luaState, o_mesh);
	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::CreateMesh(cMesh*& o_mesh,
	eae6320::Graphics::VertexFormats::sVertex_mesh* vertexData,
	unsigned int vertexCount, uint16_t* indexData, unsigned int indexCount)
{
	auto result = Results::Success;
	cMesh* newMesh = nullptr;
	cScopeGuard scopeGuard([&o_mesh, &result, &newMesh]
		{
			if (result)
			{
				EAE6320_ASSERT(newMesh != nullptr);
				o_mesh = newMesh;
			}
			else
			{
				if (newMesh)
				{
					newMesh->DecrementReferenceCount();
					newMesh = nullptr;
				}
				o_mesh = nullptr;
			}
		});
	// Allocate a new mesh
	{
		newMesh = new cMesh();
		if (!newMesh)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the mesh");
			Logging::OutputError("Failed to allocate memory for the mesh");
			return result;
		}
	}
	// Initialize mesh
	if (!(result = newMesh->Initialize(vertexData, vertexCount, indexData, indexCount)))
	{
		EAE6320_ASSERTF(false, "Initialization of new mesh failed");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::LoadTableValues(lua_State& io_luaState, cMesh*& o_mesh)
{
	auto result = eae6320::Results::Success;

	/*eae6320::Graphics::VertexFormats::sVertex_mesh vertexData[1000];
	int vertexCount;
	uint16_t indexData[1000];
	int indexCount;*/

	eae6320::Graphics::VertexFormats::sVertex_mesh* vertexData = nullptr;
	unsigned int vertexCount;
	uint16_t* indexData = nullptr;
	unsigned int indexCount;

	if (!(result = LoadTableValues_Vertices(io_luaState, vertexData, vertexCount)))
	{
		Logging::OutputError("Mesh has too many vertices, cannot load mesh.");
		o_mesh = nullptr;
		return result;
	}
	if (!(result = LoadTableValues_Indices(io_luaState, indexData, indexCount)))
	{
		Logging::OutputError("Mesh has too many vertices, cannot load mesh.");
		o_mesh = nullptr;
		return result;
	}

	result = o_mesh->CreateMesh(o_mesh, vertexData, vertexCount, indexData, indexCount);

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::LoadTableValues_Vertices(lua_State& io_luaState, 
	eae6320::Graphics::VertexFormats::sVertex_mesh*& vertexData,
	unsigned int& vertexCount)
{
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "vertices";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_popVertices([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_Vertices_Paths(io_luaState, vertexData, vertexCount)))
		{
			return result;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		Logging::OutputError("The value at \"", key, "\" must be a table "
			"(instead of a ", luaL_typename(&io_luaState, -1), ")");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::LoadTableValues_Vertices_Paths(lua_State& io_luaState, 
	eae6320::Graphics::VertexFormats::sVertex_mesh*& vertexData,
	unsigned int& vertexCount)
{
	auto result = eae6320::Results::Success;

	vertexCount = (int) luaL_len(&io_luaState, -1);
	vertexData = new eae6320::Graphics::VertexFormats::sVertex_mesh[vertexCount];

	for (unsigned int i = 1; i <= vertexCount; ++i) {
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popValue([&io_luaState] { lua_pop(&io_luaState, 1); });

		lua_pushstring(&io_luaState, "x");
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popValueX([&io_luaState] { lua_pop(&io_luaState, 1); });
		const auto x = lua_tonumber(&io_luaState, -1);

		lua_pushstring(&io_luaState, "y");
		lua_gettable(&io_luaState, -3);
		eae6320::cScopeGuard scopeGuard_popValueY([&io_luaState] { lua_pop(&io_luaState, 1); });
		const auto y = lua_tonumber(&io_luaState, -1);

		lua_pushstring(&io_luaState, "z");
		lua_gettable(&io_luaState, -4);
		eae6320::cScopeGuard scopeGuard_popValueZ([&io_luaState] { lua_pop(&io_luaState, 1); });
		const auto z = lua_tonumber(&io_luaState, -1);

		lua_pushstring(&io_luaState, "r");
		lua_gettable(&io_luaState, -5);
		eae6320::cScopeGuard scopeGuard_popValueR([&io_luaState] { lua_pop(&io_luaState, 1); });
		const auto check = lua_tostring(&io_luaState, -1);
		uint8_t r = 255;
		if (lua_tostring(&io_luaState, -1))
			r = static_cast<uint8_t>(lua_tonumber(&io_luaState, -1) * 255);

		lua_pushstring(&io_luaState, "g");
		lua_gettable(&io_luaState, -6);
		eae6320::cScopeGuard scopeGuard_popValueG([&io_luaState] { lua_pop(&io_luaState, 1); });
		uint8_t g = 255;
		if (lua_tostring(&io_luaState, -1))
			g = static_cast<uint8_t>(lua_tonumber(&io_luaState, -1) * 255);

		lua_pushstring(&io_luaState, "b");
		lua_gettable(&io_luaState, -7);
		eae6320::cScopeGuard scopeGuard_popValueB([&io_luaState] { lua_pop(&io_luaState, 1); });
		uint8_t b = 255;
		if (lua_tostring(&io_luaState, -1))
			b = static_cast<uint8_t>(lua_tonumber(&io_luaState, -1) * 255);

		lua_pushstring(&io_luaState, "a");
		lua_gettable(&io_luaState, -8);
		eae6320::cScopeGuard scopeGuard_popValueA([&io_luaState] { lua_pop(&io_luaState, 1); });
		uint8_t a = 255;
		if (lua_tostring(&io_luaState, -1))
			uint8_t a = static_cast<uint8_t>(lua_tonumber(&io_luaState, -1) * 255);

		vertexData[i - 1] = { (float) x, (float) y, (float) z , (uint8_t) r, (uint8_t) g , (uint8_t) b , (uint8_t) a };
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::LoadTableValues_Indices(lua_State& io_luaState, 
	uint16_t*& indexData, unsigned int& indexCount)
{
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "indices";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_popTextures([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_Indices_Paths(io_luaState, indexData, indexCount)))
		{
			return result;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		Logging::OutputError("The value at \"", key, "\" must be a table "
			"(instead of a ", luaL_typename(&io_luaState, -1), ")");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::LoadTableValues_Indices_Paths(lua_State& io_luaState, 
	uint16_t*& indexData, unsigned int& indexCount)
{
	auto result = eae6320::Results::Success;

	const auto indexTableCount = luaL_len(&io_luaState, -1);
	const auto indexCountPerMesh = 3;
	indexCount = (unsigned int) (indexTableCount * indexCountPerMesh);
	indexData = new uint16_t[indexCount];

	for (int i = 1; i <= indexTableCount; ++i) {
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popValue([&io_luaState] { lua_pop(&io_luaState, 1); });

		constexpr auto* const key = "vertices";
		lua_pushstring(&io_luaState, key);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popValue2([&io_luaState] { lua_pop(&io_luaState, 1); });
		if (lua_istable(&io_luaState, -1))
		{
			if (!(result = LoadTableValues_Indices_Paths_Vertices(io_luaState, indexData, indexCount, 
				i + (indexCountPerMesh - 1) * (i - 1), indexCountPerMesh)))
			{
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			Logging::OutputError("The value at \"", key, "\" must be a table "
				"(instead of a ", luaL_typename(&io_luaState, -1), ")");
			return result;
		}
	}

	return result;
}

eae6320::cResult eae6320::Graphics::cMesh::LoadTableValues_Indices_Paths_Vertices(lua_State& io_luaState, 
	uint16_t*& indexData, unsigned int& indexCount, int startCount, int indexCountPerMesh)
{
	auto result = eae6320::Results::Success;

	for (int i = 1; i <= indexCountPerMesh; ++i) {
		lua_pushinteger(&io_luaState, i);
		lua_gettable(&io_luaState, -2);
		eae6320::cScopeGuard scopeGuard_popValue([&io_luaState] { lua_pop(&io_luaState, 1); });
		const int index = (int) lua_tonumber(&io_luaState, -1);
		indexData[startCount - 1 + i - 1] = index;
	}

	return result;
}

eae6320::Graphics::cMesh::~cMesh()
{
	EAE6320_ASSERT(m_referenceCount == 0);
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}