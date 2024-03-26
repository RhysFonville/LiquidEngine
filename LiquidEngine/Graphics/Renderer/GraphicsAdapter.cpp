#include "GraphicsAdapter.h"

GraphicsAdapter::GraphicsAdapter(const ComPtr<IDXGIFactory2> &factory) {
	int adapter_index = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0
	bool adapter_found = false; // set this to true when a good one was found

	// find first hardware gpu that supports d3d 12
	while (factory->EnumAdapters(adapter_index, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC desc;
		HPEW(adapter->GetDesc(&desc));

		if (wcscmp(desc.Description, L"Software Adapter") == 0) {
			// we dont want a software device
			adapter_index++;
			continue;
		}

		// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
		HPEW(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr));
		if (SUCCEEDED(hpewr)) {
			this->desc = desc;
			adapter_found = true;
			break;
		}
		adapter_index++;
	}

	if (!adapter_found) {
		throw std::exception("Adapter (GPU) not found.");
	}
}

GraphicsAdapter::GraphicsAdapter(const ComPtr<IDXGIFactory2> &factory, UINT adapter) {
	if (factory->EnumAdapters(adapter, this->adapter.GetAddressOf()) == DXGI_ERROR_NOT_FOUND) {
		throw std::exception("Adapter (GPU) not found.");
	}

	DXGI_ADAPTER_DESC desc;
	HPEW(this->adapter->GetDesc(&desc));
	this->desc = desc;
}

GraphicsAdapter::GraphicsAdapter(const ComPtr<IDXGIFactory2> &factory, const std::string &adapter) {
	int adapter_index = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0
	bool adapter_found = false;

	// find first hardware gpu that supports d3d 12
	while (factory->EnumAdapters(adapter_index, this->adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC desc;
		HPEW(this->adapter->GetDesc(&desc));
		if (wcscmp(desc.Description, string_to_wstring(adapter).c_str()) == 0) {
			this->desc = desc;
			adapter_found = true;
			break;
		}

		adapter_index++;
	}

	if (!adapter_found) {
		throw std::exception("Adapter (GPU) not found.");
	}
}

GraphicsAdapter::GraphicsAdapter(const ComPtr<IDXGIAdapter> &adapter) : adapter(adapter) {
	DXGI_ADAPTER_DESC desc;
	HPEW(this->adapter->GetDesc(&desc));
	this->desc = desc;
}

void GraphicsAdapter::operator=(const GraphicsAdapter &adapter) {
	this->adapter = adapter.adapter;
	DXGI_ADAPTER_DESC desc;
	HPEW(this->adapter->GetDesc(&desc));
	this->desc = desc;
}
