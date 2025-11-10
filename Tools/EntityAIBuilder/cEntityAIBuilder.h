#pragma once
#ifndef EAE6320_CENTITYAIBUILDER_H
#define EAE6320_CENTITYAIBUILDER_H

#include <Tools/AssetBuildLibrary/iBuilder.h>

namespace eae6320 {
	namespace Assets {
		class cEntityAIBuilder final : public iBuilder {
		private:
			cResult Build(const std::vector<std::string>& i_arguments) final;
		};
	}
}

#endif // EAE6320_CENTITYAIBUILDER_H