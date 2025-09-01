#include "cGeometry.h"

#include <Engine/Asserts/Asserts.h>
#include <Engine/Logging/Logging.h>

//eae6320::cResult eae6320::Graphics::cGeometry::Initialize() {
//	auto result = Initialize_platformSpecific();
//	EAE6320_ASSERT(result);
//	return result;
//}
//
//void eae6320::Graphics::cGeometry::DrawGeometry() {
//	DrawGeometry_platformSpecific();
//}

eae6320::Graphics::cGeometry::cGeometry()
{

}

eae6320::Graphics::cGeometry::~cGeometry()
{
	const auto result = CleanUp();
	EAE6320_ASSERT(result);
}