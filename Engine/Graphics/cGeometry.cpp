#include "cGeometry.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

eae6320::Graphics::cGeometry::cGeometry()
{

}

eae6320::Graphics::cGeometry::~cGeometry()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}