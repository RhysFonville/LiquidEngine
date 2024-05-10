#pragma once

#include <fstream>
#include <Windows.h>
#include <iostream>
#include <wrl.h>
#include <d3d12.h>
#include <vector>

using Microsoft::WRL::ComPtr;

/**
* Stores resources to delete and deletes said resources.
*/
class ResourceManager {
public:
	ResourceManager(ResourceManager &) = delete;
	void operator=(const ResourceManager &) = delete;

	static ResourceManager* get_instance() {
		if (resource_manager == nullptr) {
			resource_manager = new ResourceManager{};
		}
		return resource_manager;
	}

	void add_resource_to_release(ID3D12Resource* resource) {
		resources.push_back(resource);
	}

	void add_resource_to_release(ComPtr<ID3D12Resource> &resource) {
		resources.push_back(resource);
	}

	void release_all_resources() {
		for (auto &resource : resources) {
			resource.Reset();
		}
		resources.clear();
	}

private:
	ResourceManager() { }

	static ResourceManager* resource_manager;
	
	std::vector<ComPtr<ID3D12Resource>> resources{};
};

static ResourceManager* resource_manager = ResourceManager::get_instance();
