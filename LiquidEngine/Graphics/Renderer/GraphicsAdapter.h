#pragma once

#include <dxgi1_6.h>
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "../Debug/Throw.h"

using Microsoft::WRL::ComPtr;

class GraphicsAdapter {
public:
	GraphicsAdapter() { }
	GraphicsAdapter(const ComPtr<IDXGIFactory2> &factory);
	GraphicsAdapter(const ComPtr<IDXGIFactory2> &factory, UINT adapter);
	GraphicsAdapter(const ComPtr<IDXGIFactory2> &factory, const std::string &adapter);
	GraphicsAdapter(const ComPtr<IDXGIAdapter3> &adapter);

	void operator=(const GraphicsAdapter &adapter);

	void clean_up() { adapter.Reset(); }

	GET DXGI_ADAPTER_DESC get_desc() { return desc; }

	GET DXGI_QUERY_VIDEO_MEMORY_INFO get_memory_info();

	ComPtr<IDXGIAdapter3> adapter = nullptr;

private:
	DXGI_ADAPTER_DESC desc{};
};

