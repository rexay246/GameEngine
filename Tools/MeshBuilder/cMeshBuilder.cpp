#include "cMeshBuilder.h"

#include <Engine/Windows/Functions.h>
#include <Tools/AssetBuildLibrary/Functions.h>

eae6320::cResult eae6320::Assets::cMeshBuilder::Build(const std::vector<std::string>& i_arguments) {
	strcpy((char*)m_path_target, m_path_source);

	std::string errorMessage;
	cResult result = Windows::CopyFile(m_path_source, m_path_target,
		false, true, &errorMessage);
	if (!result) {
		Assets::OutputErrorMessageWithFileInfo(m_path_source, errorMessage.c_str());
	}
	return result;
}

