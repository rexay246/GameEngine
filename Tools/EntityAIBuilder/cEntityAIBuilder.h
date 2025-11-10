#pragma once
#ifndef EAE6320_CENTITYAIBUILDER_H
#define EAE6320_CENTITYAIBUILDER_H

#include <Tools/AssetBuildLibrary/iBuilder.h>
#include <External/Lua/Includes.h>
#include <Engine/Math/sVector.h>
#include <fstream> 

namespace eae6320 {
	namespace Assets {
		class cEntityAIBuilder final : public iBuilder {
		private:
			cResult Build(const std::vector<std::string>& i_arguments) final;
			cResult Load(const std::string& i_path);
			cResult LoadTableValues(lua_State& io_luaState);
			cResult LoadTableValues_Position(lua_State& io_luaState, Math::sVector& pos);
			cResult LoadTableValues_Speed(lua_State& io_luaState, float& walkSpeed, float& runSpeed);
			cResult LoadTableValues_BoundingBox(lua_State& io_luaState, Math::sVector& bbPos, Math::sVector& bbLen);
			cResult LoadTableValues_BBPosition(lua_State& io_luaState, Math::sVector& bbPos);
			cResult LoadTableValues_BBLength(lua_State& io_luaState, Math::sVector& bbLen);
			cResult LoadTableValues_AcceptanceRadius(lua_State& io_luaState, float& acceptanceRad);
			cResult LoadTableValues_PatrolPoints(lua_State& io_luaState, Math::sVector*& patrolPoints, unsigned int& num, float& time);
			cResult LoadTableValues_PatrolPoints_Values(lua_State& io_luaState, Math::sVector*& patrolPoints, unsigned int& num);
			cResult LoadTableValues_Chase(lua_State& io_luaState, float& detectionRange, bool& activeChase, float& maxWaitTime);
		};
	}
}

#endif // EAE6320_CENTITYAIBUILDER_H