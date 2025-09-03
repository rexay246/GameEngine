#include "cMesh.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

eae6320::Graphics::cMesh::~cMesh()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}