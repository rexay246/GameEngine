#include "cBoundingBox.h"
#include <ctime>

eae6320::EntityAI::cBoundingBox::cBoundingBox(float loc_x, float loc_y, float loc_z,
	float size_x, float size_y, float size_z)
{
	cBoundingBox(Math::sVector(loc_x, loc_y, loc_z), Math::sVector(size_x, size_y, size_z));
}

eae6320::EntityAI::cBoundingBox::cBoundingBox(Math::sVector _location, Math::sVector _size)
{
	srand((unsigned int)time(NULL));
	location = _location;
	sizes = _size;
}

bool eae6320::EntityAI::cBoundingBox::isValidPointInBoundingBox(Math::sVector location)
{
	return location.x <= getMaxXRange() &&
		location.x >= getMinXRange() &&
		location.y <= getMaxYRange() &&
		location.y >= getMinYRange() &&
		location.z <= getMaxZRange() &&
		location.z >= getMinZRange();
}

float RandomFloatWithinRange(float max, float min) {
	float random = (float)rand() / (float)RAND_MAX;
	float diff = max - min;
	return min + random * diff;
}

eae6320::Math::sVector eae6320::EntityAI::cBoundingBox::getRandomPointInBoundingBox()
{
	return Math::sVector(RandomFloatWithinRange(getMinXRange(), getMaxXRange()), 
		RandomFloatWithinRange(getMinYRange(), getMaxYRange()), 
		RandomFloatWithinRange(getMinZRange(), getMaxZRange()));
}

float eae6320::EntityAI::cBoundingBox::getMaxXRange()
{
	return location.x + sizes.x;
}

float eae6320::EntityAI::cBoundingBox::getMinXRange()
{
	return location.x - sizes.x;
}

float eae6320::EntityAI::cBoundingBox::getMaxYRange()
{
	return location.y + sizes.y;
}

float eae6320::EntityAI::cBoundingBox::getMinYRange()
{
	return location.y - sizes.y;
}

float eae6320::EntityAI::cBoundingBox::getMaxZRange()
{
	return location.z + sizes.z;
}

float eae6320::EntityAI::cBoundingBox::getMinZRange()
{
	return location.z - sizes.z;
}
