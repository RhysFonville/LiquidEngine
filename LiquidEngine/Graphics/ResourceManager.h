#pragma once

#include <vector>
#include "../globalutil.h"

namespace ResourceManager {
	namespace Release {

		static std::vector<ComPtr<ID3D12Resource>> resources;

		static void release_all_resources() {
			for (auto &resource : resources) {
				resource.Reset();
			}
		}
	};
};
