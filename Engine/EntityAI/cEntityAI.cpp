#include "cEntityAI.h"
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Logging/Logging.h>

eae6320::EntityAI::cEntityAI::cEntityAI(Math::sVector position, float WalkSpeed, float RunSpeed, 
	cBoundingBox* boundingBox, float acceptanceRadius, 
	Math::sVector* patrolPoints, int numOfPoints, 
	float detectionRange, bool activeChase, float maxWaitTime) : eae6320::EntityAI::cEntity(position, WalkSpeed)
{
	BoundingBox = boundingBox;

	if (acceptanceRadius > 1.)
		acceptanceRadius = 1.f;
	if (acceptanceRadius < 0.)
		acceptanceRadius = 0.f;
	AcceptanceRadius = acceptanceRadius;

	RunSpeed = RunSpeed;
	SetPatrolPoints(patrolPoints, numOfPoints);
	SetDetectionRange(detectionRange);
	SetActiveChase(activeChase);
	SetMaxWaitTime(maxWaitTime);
}

eae6320::EntityAI::cEntityAI::~cEntityAI()
{
	EAE6320_ASSERT(m_referenceCount == 0);
	CleanUp();
}

eae6320::cResult eae6320::EntityAI::cEntityAI::Initialize(cEntityAI*& entityAI, Math::sVector position, 
	float WalkSpeed, float RunSpeed, cBoundingBox* boundingBox, float acceptanceRadius, 
	Math::sVector* patrolPoints, int numOfPoints, float detectionRange, bool activeChase, float maxWaitTime)
{
	cResult result = Results::Success;
	cEntityAI* newEntityAI = nullptr;
	cScopeGuard scopeGuard([&entityAI, &result, &newEntityAI]
		{
			if (result)
			{
				EAE6320_ASSERT(newEntityAI != nullptr);
				entityAI = newEntityAI;
			}
			else
			{
				if (newEntityAI)
				{
					newEntityAI->DecrementReferenceCount();
					newEntityAI = nullptr;
				}
				entityAI = nullptr;
			}
		});
	// Allocate and initialize a new entityAI
	{
		newEntityAI = new cEntityAI(position, WalkSpeed, RunSpeed, 
			boundingBox, acceptanceRadius, 
			patrolPoints, numOfPoints, detectionRange, activeChase, maxWaitTime);
		if (!newEntityAI)
		{
			result = Results::OutOfMemory;
			EAE6320_ASSERTF(false, "Couldn't allocate memory for the mesh");
			Logging::OutputError("Failed to allocate memory for the mesh");
			return result;
		}
	}
	return result;
}

void eae6320::EntityAI::cEntityAI::SetPatrolPoints(Math::sVector* patrolPoints, int numOfPoints) {
	NumberOfPatrolPoints = numOfPoints;
	PatrolPoints = new Math::sVector[numOfPoints];
	memcpy(PatrolPoints, patrolPoints, sizeof(patrolPoints[0]) * NumberOfPatrolPoints);
}

void eae6320::EntityAI::cEntityAI::SetDetectionRange(float detectionRange) {
	DetectionRange = detectionRange;
}

void eae6320::EntityAI::cEntityAI::SetMaxWaitTime(float waitTime)
{
	MaxWaitTime = waitTime;
}

void eae6320::EntityAI::cEntityAI::SetActiveChase(bool active)
{
	ChaseActive = active;
}

void eae6320::EntityAI::cEntityAI::SetChaseTarget(Math::sVector* chaseTargetPosition)
{
	chaseTarget = chaseTargetPosition;
}

void eae6320::EntityAI::cEntityAI::Patrol(float elapsedTime) {
	if (NumberOfPatrolPoints <= 0)
		return;
	if (CurrentState == EnemyStates::Idle && BoundingBox) {
		CurrentPatrolIndex++;
		if (CurrentPatrolIndex >= NumberOfPatrolPoints)
			CurrentPatrolIndex = 0;
	}
	if (PatrolPoints)
	{
		CurrentMovementType = EnemyMovementType::Patrol;
		MoveTo(PatrolPoints[CurrentPatrolIndex], elapsedTime);
	}
}

void eae6320::EntityAI::cEntityAI::Chase(Math::sVector position, float elapsedTime) {
	CurrentMovementType = EnemyMovementType::Chase;
	if (!MoveTo(position, elapsedTime)) {
		MoveTo(CurTargetLocation, elapsedTime);
	}
}

void eae6320::EntityAI::cEntityAI::ChaseOrPatrol(Math::sVector position, float elapsedTime) {
	if (IsNearPosition(position)) {
		// Detect Player
		SetSpeed(RunSpeed);
		Chase(position, elapsedTime);
		WaitTime = MaxWaitTime;
	}
	else {
		if (CurrentMovementType == EnemyMovementType::Chase && 
			CurrentState == EnemyStates::Moving) {
			MoveTo(CurTargetLocation, elapsedTime);
		}
		else if (WaitTime > 0.f) {
			Idle();
			WaitTime -= elapsedTime;
			FindClosestPatrolRoute();
		}
		else {
			SetSpeed(WalkSpeed);
			Patrol(elapsedTime);
		}
	}
}

void eae6320::EntityAI::cEntityAI::FindClosestPatrolRoute() {
	Math::sVector currentPos = GetPosition();
	unsigned int indexForPatrolPoint = 0;
	float shortertDistance = 999999;
	for (unsigned int i = 0; i < NumberOfPatrolPoints; i++) {
		if (shortertDistance > (currentPos - PatrolPoints[i]).GetLength()) {
			shortertDistance = (currentPos - PatrolPoints[i]).GetLength();
			indexForPatrolPoint = i;
		}
	}
	CurrentPatrolIndex = indexForPatrolPoint - 1;
}

void eae6320::EntityAI::cEntityAI::MoveRandomly(float elapsedTime) {
	if (!BoundingBox)
		return;
	if (CurrentState == EnemyStates::Idle)
		CurTargetLocation = BoundingBox->getRandomPointInBoundingBox();
	MoveTo(CurTargetLocation, elapsedTime);
}

float RandomFloat(float max, float min) {
	float random = (float)rand() / (float)RAND_MAX;
	float diff = max - min;
	return min + random * diff;
}

void eae6320::EntityAI::cEntityAI::MoveRandomlyBouncing(float elapsedTime) {
	if (!BoundingBox)
		return;
	Math::sVector currentPos = GetPosition();
	Math::sVector currentVelocity = GetVelocity();
	if (CurrentState == EnemyStates::Idle) {
		currentVelocity = { RandomFloat(0, 1),
			RandomFloat(0, 1), 0 };
		currentVelocity.Normalize();
	}
	if (currentPos.x > BoundingBox->getMaxXRange()) {
		currentVelocity = currentVelocity - 2 * Math::Dot(currentVelocity, Math::sVector(-1, 0, 0)) * Math::sVector(-1, 0, 0);
		currentVelocity.x += RandomFloat(-0.5, 0.5);
	}
	if (currentPos.x < BoundingBox->getMinXRange()) {
		currentVelocity = currentVelocity - 2 * Math::Dot(currentVelocity, Math::sVector(1, 0, 0)) * Math::sVector(1, 0, 0);
		currentVelocity.x += RandomFloat(-0.5, 0.5);
	}
	if (currentPos.y > BoundingBox->getMaxYRange()) {
		currentVelocity = currentVelocity - 2 * Math::Dot(currentVelocity, Math::sVector(0, -1, 0)) * Math::sVector(0, -1, 0);
		currentVelocity.x += RandomFloat(-0.5, 0.5);
	}
	if (currentPos.y < BoundingBox->getMinYRange()) {
		currentVelocity = currentVelocity - 2 * Math::Dot(currentVelocity, Math::sVector(0, 1, 0)) * Math::sVector(0, 1, 0);
		currentVelocity.x += RandomFloat(-0.5, 0.5);
	}
	Move(currentVelocity, elapsedTime);
}

bool IsItCloseEnough(eae6320::Math::sVector pos1, eae6320::Math::sVector pos2, float AcceptanceRadius) {
	return (pos1.x <= pos2.x + AcceptanceRadius && pos1.x >= pos2.x - AcceptanceRadius &&
		pos1.y <= pos2.y + AcceptanceRadius && pos1.y >= pos2.y - AcceptanceRadius &&
		pos1.z <= pos2.z + AcceptanceRadius && pos1.z >= pos2.z - AcceptanceRadius);
}

bool eae6320::EntityAI::cEntityAI::IsNearPosition(Math::sVector position) {
	return IsItCloseEnough(GetPosition(), position, DetectionRange);
}

void eae6320::EntityAI::cEntityAI::Move(Math::sVector vector, float elapsedTime) {
	CurrentState = EnemyStates::Moving;
	if (vector.GetLength() > 1 - AcceptanceRadius)
		vector = vector.GetNormalized() * GetSpeed();
	else {
		vector = vector * GetSpeed();
	}
	SetVelocity(vector);
}

bool eae6320::EntityAI::cEntityAI::MoveTo(Math::sVector position, float elapsedTime) {
	if (!BoundingBox->isValidPointInBoundingBox(position))
	{
		Idle();
		return false;
	}
	Math::sVector currentPos = GetPosition();
	if (IsItCloseEnough(currentPos, position, AcceptanceRadius)) {
		Idle();
		return true;
	}
	CurTargetLocation = position;
	Math::sVector movementVector = CurTargetLocation - currentPos;
	Move(movementVector, elapsedTime);
	return true;
}

void eae6320::EntityAI::cEntityAI::MoveTo(float x, float y, float z, float elapsedTime) {
	MoveTo(Math::sVector(x, y, z), elapsedTime);
}

void eae6320::EntityAI::cEntityAI::Idle() {
	CurrentState = EnemyStates::Idle;
	SetVelocity(Math::sVector(0, 0, 0));
}

eae6320::cResult eae6320::EntityAI::cEntityAI::Load(cEntityAI*& entityAI, const std::string& i_path) {
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

	result = entityAI->LoadTableValues(entityAI, *luaState);
	return result;
}

eae6320::cResult eae6320::EntityAI::cEntityAI::LoadTableValues(cEntityAI*& entityAI, lua_State& io_luaState) {
	auto result = eae6320::Results::Success;

	Math::sVector startingPos = { 0, 0, 0 };
	float walkSpeed = 0.f;
	float runSpeed = 0.f;
	Math::sVector boundingBoxPosition = { 0, 0, 0 };
	Math::sVector boundingBoxLength = { 0, 0, 0 };
	float acceptanceRadius = 0.f;
	Math::sVector* patrolPoints = nullptr;
	unsigned int numOfPatrolPoints = 0;
	float detectionRange = 0.f;
	bool activeChase = false;
	float maxWaitTime = 0.f;

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
	if (!(result = LoadTableValues_PatrolPoints(io_luaState, patrolPoints, numOfPatrolPoints))) {
		return result;
	}
	if (!(result = LoadTableValues_Chase(io_luaState, detectionRange, activeChase, maxWaitTime))) {
		return result;
	}

	cBoundingBox* boundingBox = new cBoundingBox(boundingBoxPosition, boundingBoxLength);
	result = Initialize(entityAI, startingPos, walkSpeed, runSpeed, boundingBox, acceptanceRadius,
		patrolPoints, numOfPatrolPoints, detectionRange, activeChase, maxWaitTime);
	return result;
}

eae6320::cResult eae6320::EntityAI::cEntityAI::LoadTableValues_Position(lua_State& io_luaState, Math::sVector& pos) {
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

		pos = Math::sVector{ (float) x, (float) y, (float) z };
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

eae6320::cResult eae6320::EntityAI::cEntityAI::LoadTableValues_Speed(lua_State& io_luaState, float& walkSpeed, float& runSpeed) {
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

eae6320::cResult eae6320::EntityAI::cEntityAI::LoadTableValues_BoundingBox(lua_State& io_luaState, Math::sVector& bbPos, Math::sVector& bbLen) {
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

eae6320::cResult eae6320::EntityAI::cEntityAI::LoadTableValues_BBPosition(lua_State& io_luaState, Math::sVector& bbPos) {
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

eae6320::cResult eae6320::EntityAI::cEntityAI::LoadTableValues_BBLength(lua_State& io_luaState, Math::sVector& bbLen) {
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

eae6320::cResult eae6320::EntityAI::cEntityAI::LoadTableValues_AcceptanceRadius(lua_State& io_luaState, float& acceptanceRad) {
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

eae6320::cResult eae6320::EntityAI::cEntityAI::LoadTableValues_PatrolPoints(lua_State& io_luaState, Math::sVector*& patrolPoints, 
	unsigned int& num) {
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

eae6320::cResult eae6320::EntityAI::cEntityAI::LoadTableValues_PatrolPoints_Values(lua_State& io_luaState, Math::sVector*& patrolPoints, 
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

eae6320::cResult eae6320::EntityAI::cEntityAI::LoadTableValues_Chase(lua_State& io_luaState, float& detectionRange, bool& activeChase, 
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
		result = eae6320::Results::InvalidFile;
		//Assets::OutputErrorMessageWithFileInfo(m_path_target, "The value at \"", key, "\" must be a table "
		//	"(instead of a ", luaL_typename(&io_luaState, -1), ")");
		return result;
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
		result = eae6320::Results::InvalidFile;
		//Assets::OutputErrorMessageWithFileInfo(m_path_target, "The value at \"", key, "\" must be a table "
		//	"(instead of a ", luaL_typename(&io_luaState, -1), ")");
		return result;
	}

	constexpr auto* const key3 = "MaxWaitTime";
	lua_pushstring(&io_luaState, key3);
	lua_gettable(&io_luaState, -4);
	eae6320::cScopeGuard scopeGuard_popMaxWaitTime([&io_luaState] { lua_pop(&io_luaState, 1); });
	if (lua_isnumber(&io_luaState, -1)) {
		const auto x = lua_tonumber(&io_luaState, -1);
		maxWaitTime = (float)x;
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