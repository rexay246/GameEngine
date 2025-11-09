#include "cEntityAI.h"

eae6320::EntityAI::cEntityAI::cEntityAI(Math::sVector position, float WalkSpeed, float RunSpeed, 
	cBoundingBox* boundingBox, float acceptanceRadius, 
	Math::sVector* patrolPoints, int numOfPoints, 
	float detectionRange) : eae6320::EntityAI::cEntity(position, WalkSpeed)
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
}

eae6320::EntityAI::cEntityAI::cEntityAI(Math::sVector position, float WalkSpeed,
	float RunSpeed, cBoundingBox* boundingBox = nullptr,
	float acceptanceRadius) :
	eae6320::EntityAI::cEntity(position, WalkSpeed) {
	BoundingBox = boundingBox;

	if (acceptanceRadius > 1.)
		acceptanceRadius = 1.f;
	if (acceptanceRadius < 0.)
		acceptanceRadius = 0.f;
	AcceptanceRadius = acceptanceRadius;

	RunSpeed = RunSpeed;
}

void eae6320::EntityAI::cEntityAI::SetPatrolPoints(Math::sVector* patrolPoints, int numOfPoints) {
	NumberOfPatrolPoints = numOfPoints;
	PatrolPoints = new Math::sVector[numOfPoints];
	memcpy(PatrolPoints, patrolPoints, sizeof(patrolPoints[0]) * NumberOfPatrolPoints);
}

void eae6320::EntityAI::cEntityAI::SetDetectionRange(float detectionRange) {
	DetectionRange = detectionRange;
}

void eae6320::EntityAI::cEntityAI::Patrol() {
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
		MoveTo(PatrolPoints[CurrentPatrolIndex]);
	}
}

void eae6320::EntityAI::cEntityAI::Chase(Math::sVector position) {
	CurrentMovementType = EnemyMovementType::Chase;
	if (!MoveTo(position)) {
		MoveTo(CurTargetLocation);
	}
}

void eae6320::EntityAI::cEntityAI::ChaseOrPatrol(Math::sVector position, float elapsedTime) {
	if (IsNearPosition(position)) {
		// Detect Player
		SetSpeed(RunSpeed);
		Chase(position);
		WaitTime = MaxWaitTime;
	}
	else {
		if (CurrentMovementType == EnemyMovementType::Chase && 
			CurrentState == EnemyStates::Moving) {
			MoveTo(CurTargetLocation);
		}
		else if (WaitTime > 0.f) {
			Idle();
			WaitTime -= elapsedTime;
			FindClosestPatrolRoute();
		}
		else {
			SetSpeed(WalkSpeed);
			Patrol();
		}
	}
}

void eae6320::EntityAI::cEntityAI::FindClosestPatrolRoute() {
	Math::sVector currentPos = GetPosition();
	int indexForPatrolPoint = 0;
	float shortertDistance = 999999;
	for (int i = 0; i < NumberOfPatrolPoints; i++) {
		if (shortertDistance > (currentPos - PatrolPoints[i]).GetLength()) {
			shortertDistance = (currentPos - PatrolPoints[i]).GetLength();
			indexForPatrolPoint = i;
		}
	}
	CurrentPatrolIndex = indexForPatrolPoint - 1;
}

void eae6320::EntityAI::cEntityAI::MoveRandomly() {
	if (!BoundingBox)
		return;
	if (CurrentState == EnemyStates::Idle)
		CurTargetLocation = BoundingBox->getRandomPointInBoundingBox();
	MoveTo(CurTargetLocation);
}

float RandomFloat(float max, float min) {
	float random = (float)rand() / (float)RAND_MAX;
	float diff = max - min;
	return min + random * diff;
}

void eae6320::EntityAI::cEntityAI::MoveRandomlyBouncing() {
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
	Move(currentVelocity);
}

bool IsItCloseEnough(eae6320::Math::sVector pos1, eae6320::Math::sVector pos2, float AcceptanceRadius) {
	return (pos1.x <= pos2.x + AcceptanceRadius && pos1.x >= pos2.x - AcceptanceRadius &&
		pos1.y <= pos2.y + AcceptanceRadius && pos1.y >= pos2.y - AcceptanceRadius &&
		pos1.z <= pos2.z + AcceptanceRadius && pos1.z >= pos2.z - AcceptanceRadius);
}

bool eae6320::EntityAI::cEntityAI::IsNearPosition(Math::sVector position) {
	return IsItCloseEnough(GetPosition(), position, DetectionRange);
}

void eae6320::EntityAI::cEntityAI::Move(Math::sVector vector) {
	CurrentState = EnemyStates::Moving;
	if (vector.GetLength() > 1 - AcceptanceRadius)
		vector = vector.GetNormalized() * GetSpeed();
	else {
		vector = vector * GetSpeed();
	}
	SetVelocity(vector);
}

bool eae6320::EntityAI::cEntityAI::MoveTo(Math::sVector position) {
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
	Move(movementVector);
	return true;
}

void eae6320::EntityAI::cEntityAI::MoveTo(float x, float y, float z) {
	MoveTo(Math::sVector(x, y, z));
}

void eae6320::EntityAI::cEntityAI::Idle() {
	CurrentState = EnemyStates::Idle;
	SetVelocity(Math::sVector(0, 0, 0));
}