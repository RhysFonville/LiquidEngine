#include "GraphicsAdapter.h"

GraphicsAdapter::GraphicsAdapter(const ComPtr<IDXGIFactory2> &factory) {
	int adapter_index = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0
	bool adapter_found = false; // set this to true when a good one was found

	// find first hardware gpu that supports d3d 12
	IDXGIAdapter1* adapter1{};
	while (factory->EnumAdapters1(adapter_index, &adapter1) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC desc;
		HPEW(adapter1->GetDesc(&desc));

		if (wcscmp(desc.Description, L"Software Adapter") == 0) {
			// we dont want a software device
			adapter_index++;
			continue;
		}

		// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
		HPEW(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr));
		if (SUCCEEDED(hpewr)) {
			adapter_found = true;
			this->desc = desc;
			HPEW(adapter1->QueryInterface(this->adapter.GetAddressOf()));
			adapter1->Release();
			break;
		}
		adapter_index++;
	}

	if (!adapter_found) {
		throw std::exception("Adapter (GPU) not found.");
	}
}

GraphicsAdapter::GraphicsAdapter(const ComPtr<IDXGIFactory2> &factory, UINT adapter) {
	IDXGIAdapter1* adapter1{};
	if (factory->EnumAdapters1(adapter, &adapter1) == DXGI_ERROR_NOT_FOUND) {
		throw std::exception("Adapter (GPU) not found.");
	}

	HPEW(adapter1->QueryInterface(this->adapter.GetAddressOf()));
	adapter1->Release();

	DXGI_ADAPTER_DESC desc{};
	HPEW(this->adapter->GetDesc(&desc));
	this->desc = desc;
}

GraphicsAdapter::GraphicsAdapter(const ComPtr<IDXGIFactory2> &factory, const std::string &adapter) {
	int adapter_index = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0
	bool adapter_found = false;

	// find first hardware gpu that supports d3d 12
	IDXGIAdapter1* adapter1{};
	while (factory->EnumAdapters1(adapter_index, &adapter1) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC desc{};
		HPEW(adapter1->GetDesc(&desc));
		if (wcscmp(desc.Description, string_to_wstring(adapter).c_str()) == 0) {
			adapter_found = true;
			this->desc = desc;
			HPEW(adapter1->QueryInterface(this->adapter.GetAddressOf()));
			adapter1->Release();
			break;
		}

		adapter_index++;
	}

	if (!adapter_found) {
		throw std::exception("Adapter (GPU) not found.");
	}
}

GraphicsAdapter::GraphicsAdapter(const ComPtr<IDXGIAdapter3> &adapter) : adapter(adapter) {
	DXGI_ADAPTER_DESC desc{};
	HPEW(this->adapter->GetDesc(&desc));
	this->desc = desc;
}

void GraphicsAdapter::operator=(const GraphicsAdapter &adapter) {
	this->adapter = adapter.adapter;
	DXGI_ADAPTER_DESC desc{};
	HPEW(this->adapter->GetDesc(&desc));
	this->desc = desc;
}


DXGI_QUERY_VIDEO_MEMORY_INFO GraphicsAdapter::get_memory_info() {
	DXGI_QUERY_VIDEO_MEMORY_INFO info{};
	HPEW(adapter->QueryVideoMemoryInfo(0u, DXGI_MEMORY_SEGMENT_GROUP_LOCAL, &info));
	return info;
}
