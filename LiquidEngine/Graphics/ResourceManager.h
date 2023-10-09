#pragma once

#include <vector>
#include "../globalutil.h"

namespace ResourceManager {
	namespace Release {

		static std::vector<ID3D12Resource*> resources;

		static void release_all_resources() {
			for (ID3D12Resource* resource : resources) {
				resource->Release();
			}
		}
	};
};
