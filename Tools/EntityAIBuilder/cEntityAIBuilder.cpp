#include "cEntityAIBuilder.h"

#include <Engine/Windows/Functions.h>
#include <Tools/AssetBuildLibrary/Functions.h>
#include <Engine/ScopeGuard/cScopeGuard.h>

eae6320::cResult eae6320::Assets::cEntityAIBuilder::Build(const std::vector<std::string>& i_arguments) {
	std::string errorMessage;
	cResult result = Windows::CopyFile(m_path_source, m_path_target,
		false, true, &errorMessage);
	if (!result) {
		Assets::OutputErrorMessageWithFileInfo(m_path_source, errorMessage.c_str());
	}
	result = Load(m_path_source);
	return result;
}

eae6320::cResult eae6320::Assets::cEntityAIBuilder::Load(const std::string& i_path) {
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
				//EAE6320_ASSERT(lua_gettop(luaState) == 0);

				lua_close(luaState);
				luaState = nullptr;
			}
		});
	{
		luaState = luaL_newstate();
		if (!luaState)
		{
			result = eae6320::Results::OutOfMemory;
			//Assets::OutputErrorMessageWithFileInfo(m_path_target, "Failed to create a new Lua state");
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
			//Assets::OutputErrorMessageWithFileInfo(m_path_target, lua_tostring(luaState, -1));
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
					//Assets::OutputErrorMessageWithFileInfo(m_path_target, "Asset files must return a table (instead of a ",
					//	luaL_typename(luaState, -1), ")");
					// Pop the returned non-table value
					lua_pop(luaState, 1);
					return result;
				}
			}
			else
			{
				result = eae6320::Results::InvalidFile;
				//Assets::OutputErrorMessageWithFileInfo(m_path_target, "Asset files must return a single table (instead of ",
				//	returnedValueCount, " values)");
				// Pop every value that was returned
				lua_pop(luaState, returnedValueCount);
				return result;
			}
		}
		else
		{
			result = eae6320::Results::InvalidFile;
			//Assets::OutputErrorMessageWithFileInfo(m_path_target, lua_tostring(luaState, -1));
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

	result = LoadTableValues(*luaState);
	return result;
}

eae6320::cResult eae6320::Assets::cEntityAIBuilder::LoadTableValues(lua_State& io_luaState) {
	auto result = eae6320::Results::Success;

	Math::sVector startingPos = { 0, 0, 0 };
	float walkSpeed = 0.f;
	float runSpeed = 0.f;
	Math::sVector boundingBoxPosition = { 0, 0, 0 };
	Math::sVector boundingBoxLength = { 0, 0, 0 };
	float acceptanceRadius = 0.f;
	Math::sVector* patrolPoints = nullptr;
	unsigned int numOfPatrolPoints = 0;
	float maxPatrolWaitTime = 0.f;
	float detectionRange = 0.f;
	bool activeChase = false;
	float maxChaseWaitTime = 0.f;

	if (!(result = LoadTableValues_Position(io_luaState, startingPos))) {
		return result;
	}
	if (!(result = LoadTableValues_Speed(io_luaState, walkSpeed, runSpeed))) {
		return result;
	}
	if (!(result = LoadTableValues_BoundingBox(io_luaState, boundingBoxPosition, boundingBoxLength))) {
		return result;
	}
	if (!(result = LoadTableValues_AcceptanceRadius(io_luaState, acceptanceRadius))) {
		return result;
	}
	if (!(result = LoadTableValues_PatrolPoints(io_luaState, patrolPoints, numOfPatrolPoints, maxPatrolWaitTime))) {
		return result;
	}
	if (!(result = LoadTableValues_Chase(io_luaState, detectionRange, activeChase, maxChaseWaitTime))) {
		return result;
	}

	std::ofstream outfile(m_path_target, std::ofstream::binary);
	outfile.write(reinterpret_cast<const char*>(&startingPos), sizeof(Math::sVector));
	outfile.write(reinterpret_cast<const char*>(&walkSpeed), sizeof(float));
	outfile.write(reinterpret_cast<const char*>(&runSpeed), sizeof(float));
	outfile.write(reinterpret_cast<const char*>(&boundingBoxPosition), sizeof(Math::sVector));
	outfile.write(reinterpret_cast<const char*>(&boundingBoxLength), sizeof(Math::sVector));
	outfile.write(reinterpret_cast<const char*>(&acceptanceRadius), sizeof(float));
	outfile.write(reinterpret_cast<const char*>(&numOfPatrolPoints), sizeof(uint16_t));
	outfile.write(reinterpret_cast<const char*>(patrolPoints), sizeof(Math::sVector) * numOfPatrolPoints);
	outfile.write(reinterpret_cast<const char*>(&maxPatrolWaitTime), sizeof(float));
	outfile.write(reinterpret_cast<const char*>(&detectionRange), sizeof(float));
	outfile.write(reinterpret_cast<const char*>(&activeChase), sizeof(bool));
	outfile.write(reinterpret_cast<const char*>(&maxChaseWaitTime), sizeof(float));
	outfile.close();

	return result;
}

eae6320::cResult eae6320::Assets::cEntityAIBuilder::LoadTableValues_Position(lua_State& io_luaState, Math::sVector& pos) {
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "StartPosition";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_pop_Positions([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	if (lua_istable(&io_luaState, -1))
	{
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

		pos = Math::sVector{ (float)x, (float)y, (float)z };
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		//Assets::OutputErrorMessageWithFileInfo(m_path_target, "The value at \"", key, "\" must be a table "
		//	"(instead of a ", luaL_typename(&io_luaState, -1), ")");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cEntityAIBuilder::LoadTableValues_Speed(lua_State& io_luaState, float& walkSpeed, float& runSpeed) {
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "WalkSpeed";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	eae6320::cScopeGuard scopeGuard_popWalkSpeed([&io_luaState] { lua_pop(&io_luaState, 1); });
	if (lua_isnumber(&io_luaState, -1)) {
		const auto x = lua_tonumber(&io_luaState, -1);
		walkSpeed = (float)x;
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		//Assets::OutputErrorMessageWithFileInfo(m_path_target, "The value at \"", key, "\" must be a table "
		//	"(instead of a ", luaL_typename(&io_luaState, -1), ")");
		return result;
	}

	constexpr auto* const key2 = "RunSpeed";
	lua_pushstring(&io_luaState, key2);
	lua_gettable(&io_luaState, -3);
	eae6320::cScopeGuard scopeGuard_popRunSpeed([&io_luaState] { lua_pop(&io_luaState, 1); });
	if (lua_isnumber(&io_luaState, -1)) {
		const auto x = lua_tonumber(&io_luaState, -1);
		runSpeed = (float)x;
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		//Assets::OutputErrorMessageWithFileInfo(m_path_target, "The value at \"", key, "\" must be a table "
		//	"(instead of a ", luaL_typename(&io_luaState, -1), ")");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cEntityAIBuilder::LoadTableValues_BoundingBox(lua_State& io_luaState, Math::sVector& bbPos, Math::sVector& bbLen) {
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "BoundingBox";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_pop_Positions([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_BBPosition(io_luaState, bbPos)))
		{
			return result;
		}
		if (!(result = LoadTableValues_BBLength(io_luaState, bbLen)))
		{
			return result;
		}
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		//Assets::OutputErrorMessageWithFileInfo(m_path_target, "The value at \"", key, "\" must be a table "
		//	"(instead of a ", luaL_typename(&io_luaState, -1), ")");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cEntityAIBuilder::LoadTableValues_BBPosition(lua_State& io_luaState, Math::sVector& bbPos) {
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "Position";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_pop_Positions([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	if (lua_istable(&io_luaState, -1))
	{
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

		bbPos = Math::sVector{ (float)x, (float)y, (float)z };
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		//Assets::OutputErrorMessageWithFileInfo(m_path_target, "The value at \"", key, "\" must be a table "
		//	"(instead of a ", luaL_typename(&io_luaState, -1), ")");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cEntityAIBuilder::LoadTableValues_BBLength(lua_State& io_luaState, Math::sVector& bbLen) {
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "Length";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_pop_Lengths([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	if (lua_istable(&io_luaState, -1))
	{
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

		bbLen = Math::sVector{ (float)x, (float)y, (float)z };
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		//Assets::OutputErrorMessageWithFileInfo(m_path_target, "The value at \"", key, "\" must be a table "
		//	"(instead of a ", luaL_typename(&io_luaState, -1), ")");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cEntityAIBuilder::LoadTableValues_AcceptanceRadius(lua_State& io_luaState, float& acceptanceRad) {
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "AcceptanceRadius";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	eae6320::cScopeGuard scopeGuard_popAcceptanceRadius([&io_luaState] { lua_pop(&io_luaState, 1); });
	if (lua_isnumber(&io_luaState, -1)) {
		const auto x = lua_tonumber(&io_luaState, -1);
		acceptanceRad = (float)x;
	}
	else
	{
		result = eae6320::Results::InvalidFile;
		//Assets::OutputErrorMessageWithFileInfo(m_path_target, "The value at \"", key, "\" must be a table "
		//	"(instead of a ", luaL_typename(&io_luaState, -1), ")");
		return result;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cEntityAIBuilder::LoadTableValues_PatrolPoints(lua_State& io_luaState, Math::sVector*& patrolPoints,
	unsigned int& num, float& time) {
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "PatrolPoints";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);

	eae6320::cScopeGuard scopeGuard_popVertices([&io_luaState]
		{
			lua_pop(&io_luaState, 1);
		});

	if (lua_istable(&io_luaState, -1))
	{
		if (!(result = LoadTableValues_PatrolPoints_Values(io_luaState, patrolPoints, num)))
		{
			patrolPoints = nullptr;
			num = 0;
		}
	}
	else
	{
		patrolPoints = nullptr;
		num = 0;
	}

	if (num <= 0) {
		time = 0;
		return result;
	}

	constexpr auto* const key2 = "MaxPatrolWaitTime";
	lua_pushstring(&io_luaState, key2);
	lua_gettable(&io_luaState, -3);
	eae6320::cScopeGuard scopeGuard_popWalkSpeed([&io_luaState] { lua_pop(&io_luaState, 1); });
	if (lua_isnumber(&io_luaState, -1)) {
		const auto x = lua_tonumber(&io_luaState, -1);
		time = (float)x;
	}

	return result;
}

eae6320::cResult eae6320::Assets::cEntityAIBuilder::LoadTableValues_PatrolPoints_Values(lua_State& io_luaState, Math::sVector*& patrolPoints,
	unsigned int& num) {
	auto result = eae6320::Results::Success;

	num = (unsigned int)luaL_len(&io_luaState, -1);
	patrolPoints = new Math::sVector[num];

	for (unsigned int i = 1; i <= num; ++i) {
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

		patrolPoints[i - 1] = { (float)x, (float)y, (float)z };
	}

	return result;
}

eae6320::cResult eae6320::Assets::cEntityAIBuilder::LoadTableValues_Chase(lua_State& io_luaState, float& detectionRange, bool& activeChase,
	float& maxWaitTime) {
	auto result = eae6320::Results::Success;

	constexpr auto* const key = "ActiveChase";
	lua_pushstring(&io_luaState, key);
	lua_gettable(&io_luaState, -2);
	eae6320::cScopeGuard scopeGuard_popActiveChase([&io_luaState] { lua_pop(&io_luaState, 1); });
	if (lua_isboolean(&io_luaState, -1)) {
		const auto x = lua_toboolean(&io_luaState, -1);
		activeChase = (bool)x;
	}
	else
	{
		activeChase = false;
	}

	constexpr auto* const key2 = "DetectionRange";
	lua_pushstring(&io_luaState, key2);
	lua_gettable(&io_luaState, -3);
	eae6320::cScopeGuard scopeGuard_popWalkSpeed([&io_luaState] { lua_pop(&io_luaState, 1); });
	if (lua_isnumber(&io_luaState, -1)) {
		const auto x = lua_tonumber(&io_luaState, -1);
		detectionRange = (float)x;
	}
	else
	{
		detectionRange = 0.f;
	}

	constexpr auto* const key3 = "MaxChaseWaitTime";
	lua_pushstring(&io_luaState, key3);
	lua_gettable(&io_luaState, -4);
	eae6320::cScopeGuard scopeGuard_popMaxWaitTime([&io_luaState] { lua_pop(&io_luaState, 1); });
	if (lua_isnumber(&io_luaState, -1)) {
		const auto x = lua_tonumber(&io_luaState, -1);
		maxWaitTime = (float)x;
	}
	else
	{
		maxWaitTime = 0.f;
	}

	return result;
}