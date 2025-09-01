#pragma once

#ifndef EAE6320_GRAPHICS_CEFFECT_H
#define EAE6320_GRAPHICS_CEFFECT_H

#include "Configuration.h"

#include <cstdint>
#include <Engine/Results/Results.h>

namespace eae6320 {
	namespace Graphics {
		class cEffect {
		public:
			void BindingEffect();
			cResult Initialize();
			cResult CleanUp();
		private:
		};
	}
}

#endif	// EAE6320_GRAPHICS_CEFFECT_H