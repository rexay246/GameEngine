#include "cEntityAI.h"

eae6320::EntityAI::cEntityAI::cEntityAI(Math::sVector position, float speed, 
	eae6320::EntityAI::cBoundingBox* boundingBox, float acceptanceRadius) : 
	eae6320::EntityAI::cEntity(position, speed) {
	BoundingBox = boundingBox;
	AcceptanceRadius = acceptanceRadius;
}

void eae6320::EntityAI::cEntityAI::MoveRandomly() {
	if (Arrived && BoundingBox)
		CurTargetLocation = BoundingBox->getRandomPointInBoundingBox();
	MoveTo(CurTargetLocation);
}

bool IsItCloseEnough(eae6320::Math::sVector pos1, eae6320::Math::sVector pos2, float AcceptanceRadius) {
	return (pos1.x <= pos2.x + AcceptanceRadius && pos1.x >= pos2.x - AcceptanceRadius &&
		pos1.y <= pos2.y + AcceptanceRadius && pos1.y >= pos2.y - AcceptanceRadius &&
		pos1.z <= pos2.z + AcceptanceRadius && pos1.z >= pos2.z - AcceptanceRadius);
}

bool eae6320::EntityAI::cEntityAI::MoveTo(Math::sVector position) {
	if (BoundingBox == nullptr || !BoundingBox->isValidPointInBoundingBox(position)) {
		Arrived = true;
		return Arrived;
	}
	if (CurTargetLocation != position) {
		CurTargetLocation = position;
	}
	Arrived = false;
	Math::sVector currentPos = GetPosition();
	Math::sVector movementVector = CurTargetLocation - currentPos;
	movementVector = movementVector.GetNormalized() * GetSpeed();
	SetVelocity(movementVector);
	if (IsItCloseEnough(currentPos, CurTargetLocation, AcceptanceRadius))
		Arrived = true;
	return Arrived;
}

void eae6320::EntityAI::cEntityAI::MoveTo(float x, float y, float z) {
	MoveTo(Math::sVector(x, y, z));
}