#include "cEntityAI.h"
#include <Engine/ScopeGuard/cScopeGuard.h>
#include <Engine/Logging/Logging.h>
#include <Engine/Platform/Platform.h>

// Helper Definitions
//===================

float RandomFloat(float max, float min) {
	float random = (float)rand() / (float)RAND_MAX;
	float diff = max - min;
	return min + random * diff;
}

bool IsItCloseEnough(eae6320::Math::sVector pos1, eae6320::Math::sVector pos2, float AcceptanceRadius) {
	return (pos1.x <= pos2.x + AcceptanceRadius && pos1.x >= pos2.x - AcceptanceRadius &&
		pos1.y <= pos2.y + AcceptanceRadius && pos1.y >= pos2.y - AcceptanceRadius &&
		pos1.z <= pos2.z + AcceptanceRadius && pos1.z >= pos2.z - AcceptanceRadius);
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
	CurrentPatrolIndex = indexForPatrolPoint;
}

bool eae6320::EntityAI::cEntityAI::IsNearPosition(Math::sVector position) {
	return IsItCloseEnough(GetPosition(), position, DetectionRange);
}

// Movement
//===================================

void eae6320::EntityAI::cEntityAI::Patrol(float elapsedTime, Math::sVector* chaseTargetPosition) {
	if (NumberOfPatrolPoints <= 0)
		return;
	if (CurrentState == EnemyStates::Idle && BoundingBox) {
		if (PatrolWaitTime > 0.f) {
			Idle();
			PatrolWaitTime -= elapsedTime;
		}
		else {
			SetSpeed(WalkSpeed);
			PatrolWaitTime = MaxPatrolWaitTime;
			CurrentPatrolIndex++;
			if (CurrentPatrolIndex >= NumberOfPatrolPoints)
				CurrentPatrolIndex = 0;
		}
	}
	if (PatrolPoints)
	{
		//CurrentMovementType = EnemyMovementType::Patrol;
		if (!MoveTo(PatrolPoints[CurrentPatrolIndex], elapsedTime, chaseTargetPosition)) {
			FindClosestPatrolRoute();
		}
	}
}

void eae6320::EntityAI::cEntityAI::Chase(Math::sVector* chaseTargetPosition, float elapsedTime)
{
	if (ChaseActive) {
		if (!(MoveTo(*chaseTargetPosition, elapsedTime))) {
			MoveTo(CurTargetLocation, elapsedTime);
		}
	}
}

void eae6320::EntityAI::cEntityAI::MoveRandomly(float elapsedTime, Math::sVector* chaseTargetPosition) {
	if (!BoundingBox)
		return;
	if (CurrentState == EnemyStates::Idle)
		CurTargetLocation = BoundingBox->getRandomPointInBoundingBox();
	MoveTo(CurTargetLocation, elapsedTime, chaseTargetPosition);
}

void eae6320::EntityAI::cEntityAI::MoveRandomlyBouncing(float elapsedTime, Math::sVector* chaseTargetPosition) {
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
	MoveInOneDirection(currentVelocity, elapsedTime, chaseTargetPosition);
}

bool eae6320::EntityAI::cEntityAI::MoveInOneDirection(Math::sVector vector, float elapsedTime, Math::sVector* chaseTargetPosition)
{
	Math::sVector currentPos = GetPosition();
	if (ChaseActive && chaseTargetPosition) {
		if (IsNearPosition(*chaseTargetPosition) &&
			BoundingBox->isValidPointInBoundingBox(*chaseTargetPosition)) {
			CurrentMovementType = EnemyMovementType::Chase;
			SetSpeed(RunSpeed);
			vector = *chaseTargetPosition - currentPos;
			CurTargetLocation = *chaseTargetPosition;
			ChaseWaitTime = MaxChaseWaitTime;
		}
		else {
			if (CurrentMovementType == EnemyMovementType::Chase &&
				CurrentState == EnemyStates::Moving) {
				vector = CurTargetLocation - currentPos;
				if (IsItCloseEnough(currentPos, CurTargetLocation, AcceptanceRadius)) {
					Idle();
					return true;
				}
			}
			else if (ChaseWaitTime > 0.f) {
				Idle();
				ChaseWaitTime -= elapsedTime;
				return false;
			}
			else {
				CurrentMovementType = EnemyMovementType::None;
				SetSpeed(WalkSpeed);
			}
		}
	}
	Move(vector, elapsedTime);
	return true;
}

bool eae6320::EntityAI::cEntityAI::MoveTo(Math::sVector position, float elapsedTime, Math::sVector* chaseTargetPosition) {
	if (ChaseActive && chaseTargetPosition) {
		if (IsNearPosition(*chaseTargetPosition) &&
			BoundingBox->isValidPointInBoundingBox(*chaseTargetPosition)) {
			CurrentMovementType = EnemyMovementType::Chase;
			SetSpeed(RunSpeed);
			position = *chaseTargetPosition;
			ChaseWaitTime = MaxChaseWaitTime;
		}
		else {
			if (CurrentMovementType == EnemyMovementType::Chase &&
				CurrentState == EnemyStates::Moving) {
				position = CurTargetLocation;
			}
			else if (ChaseWaitTime > 0.f) {
				Idle();
				ChaseWaitTime -= elapsedTime;
				return false;
			}
			else {
				CurrentMovementType = EnemyMovementType::None;
				SetSpeed(WalkSpeed);
			}
		}
	}
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

void eae6320::EntityAI::cEntityAI::Idle() {
	CurrentState = EnemyStates::Idle;
	SetVelocity(Math::sVector(0, 0, 0));
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

// Initialization and Clean Up
//===================================

eae6320::EntityAI::cEntityAI::cEntityAI(Math::sVector position, float walkSpeed, float runSpeed, 
	cBoundingBox* boundingBox, float acceptanceRadius, 
	Math::sVector* patrolPoints, int numOfPoints, 
	float detectionRange, bool activeChase, float maxPatrolWaitTime,
	float maxChaseWaitTime) : eae6320::EntityAI::cEntity(position, walkSpeed)
{
	BoundingBox = boundingBox;

	if (acceptanceRadius > 1.)
		acceptanceRadius = 1.f;
	if (acceptanceRadius < 0.)
		acceptanceRadius = 0.f;
	AcceptanceRadius = acceptanceRadius;

	WalkSpeed = GetSpeed();
	RunSpeed = runSpeed;
	SetPatrolPoints(patrolPoints, numOfPoints);
	SetDetectionRange(detectionRange);
	SetActiveChase(activeChase);
	SetMaxPatrolWaitTime(maxPatrolWaitTime);
	SetMaxChaseWaitTime(maxChaseWaitTime);
}

eae6320::EntityAI::cEntityAI::~cEntityAI()
{
	EAE6320_ASSERT(m_referenceCount == 0);
	CleanUp();
}

eae6320::cResult eae6320::EntityAI::cEntityAI::Initialize(cEntityAI*& entityAI, Math::sVector position, 
	float WalkSpeed, float RunSpeed, cBoundingBox* boundingBox, float acceptanceRadius, 
	Math::sVector* patrolPoints, int numOfPoints, float detectionRange, bool activeChase, float maxPatrolWaitTime,
	float maxChaseWaitTime)
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
			patrolPoints, numOfPoints, detectionRange, activeChase,
			maxPatrolWaitTime, maxChaseWaitTime);
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

eae6320::cResult eae6320::EntityAI::cEntityAI::Load(cEntityAI*& entityAI, const std::string& i_path)
{
	auto result = eae6320::Results::Success;
	Platform::sDataFromFile dataFromFile;
	if (!(result = Platform::LoadBinaryFile(i_path.c_str(), dataFromFile))) {
		Logging::OutputError("Error when reading from binary file");
		entityAI = nullptr;
		return result;
	}
	auto currentOffset = reinterpret_cast<uintptr_t>(dataFromFile.data);
	const auto finalOffset = currentOffset + dataFromFile.size;

	Math::sVector startingPos;
	memcpy(&startingPos, reinterpret_cast<void*>(currentOffset), sizeof(Math::sVector));
	currentOffset += sizeof(Math::sVector);

	float walkSpeed;
	memcpy(&walkSpeed, reinterpret_cast<void*>(currentOffset), sizeof(float));
	currentOffset += sizeof(float);

	float runSpeed;
	memcpy(&runSpeed, reinterpret_cast<void*>(currentOffset), sizeof(float));
	currentOffset += sizeof(float);

	Math::sVector boundingBoxPosition;
	memcpy(&boundingBoxPosition, reinterpret_cast<void*>(currentOffset), sizeof(Math::sVector));
	currentOffset += sizeof(Math::sVector);

	Math::sVector boundingBoxLength;
	memcpy(&boundingBoxLength, reinterpret_cast<void*>(currentOffset), sizeof(Math::sVector));
	currentOffset += sizeof(Math::sVector);

	float acceptanceRadius;
	memcpy(&acceptanceRadius, reinterpret_cast<void*>(currentOffset), sizeof(float));
	currentOffset += sizeof(float);

	uint16_t numOfPatrolPoints;
	memcpy(&numOfPatrolPoints, reinterpret_cast<void*>(currentOffset), sizeof(uint16_t));
	currentOffset += sizeof(uint16_t);

	auto* const patrolPoints = reinterpret_cast<Math::sVector*>(currentOffset);
	currentOffset += sizeof(Math::sVector) * numOfPatrolPoints;

	float maxPatrolWaitTime;
	memcpy(&maxPatrolWaitTime, reinterpret_cast<void*>(currentOffset), sizeof(float));
	currentOffset += sizeof(float);

	float detectionRange;
	memcpy(&detectionRange, reinterpret_cast<void*>(currentOffset), sizeof(float));
	currentOffset += sizeof(float);

	bool activeChase;
	memcpy(&activeChase, reinterpret_cast<void*>(currentOffset), sizeof(bool));
	currentOffset += sizeof(bool);

	float maxChaseWaitTime;
	memcpy(&maxChaseWaitTime, reinterpret_cast<void*>(currentOffset), sizeof(float));
	currentOffset += sizeof(float);

	cBoundingBox* boundingBox = new cBoundingBox(boundingBoxPosition, boundingBoxLength);
	result = Initialize(entityAI, startingPos, walkSpeed, runSpeed, boundingBox, acceptanceRadius,
		patrolPoints, numOfPatrolPoints, detectionRange, activeChase, maxPatrolWaitTime, maxChaseWaitTime);
	return result;
}

// Setters for variable changing
//===================================

void eae6320::EntityAI::cEntityAI::SetPatrolPoints(Math::sVector* patrolPoints, int numOfPoints) {
	NumberOfPatrolPoints = numOfPoints;
	PatrolPoints = new Math::sVector[numOfPoints];
	memcpy(PatrolPoints, patrolPoints, sizeof(patrolPoints[0]) * NumberOfPatrolPoints);
}

void eae6320::EntityAI::cEntityAI::SetDetectionRange(float detectionRange) {
	DetectionRange = detectionRange;
}

void eae6320::EntityAI::cEntityAI::SetMaxPatrolWaitTime(float waitTime)
{
	MaxPatrolWaitTime = waitTime;
}

void eae6320::EntityAI::cEntityAI::SetMaxChaseWaitTime(float waitTime)
{
	MaxChaseWaitTime = waitTime;
}

void eae6320::EntityAI::cEntityAI::SetActiveChase(bool active)
{
	ChaseActive = active;
}