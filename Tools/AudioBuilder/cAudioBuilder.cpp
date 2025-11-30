// Includes
//=========

#include "cAudioBuilder.h"

#include <Tools/AssetBuildLibrary/Functions.h>
#include "Engine/Results/cResult.h"
#include "Engine/Platform/Platform.h"

// Inherited Implementation
//=========================

// Build
//------

eae6320::cResult eae6320::Assets::cAudioBuilder::Build(const std::vector<std::string>& i_arguments)
{
    return eae6320::Platform::CopyFile(m_path_source, m_path_target, true, true);
}
