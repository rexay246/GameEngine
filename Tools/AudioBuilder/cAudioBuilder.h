/*
	This class builds shaders
*/

#ifndef EAE6320_CAUDIOBUILDER_H
#define EAE6320_CAUDIOBUILDER_H

// Includes
//=========

#include <Tools/AssetBuildLibrary/iBuilder.h>

#include <Engine/Graphics/Configuration.h>
#include <Engine/Graphics/cShader.h>

// Class Declaration
//==================

namespace eae6320
{
	namespace Assets
	{
		class cAudioBuilder final : public iBuilder
		{
			// Inherited Implementation
			//=========================

		private:

			// Build
			//------

			cResult Build(const std::vector<std::string>& i_arguments) final;

			// Implementation
			//==============
		};
	}
}

#endif	// EAE6320_CAUDIOBUILDER_H
