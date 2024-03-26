#pragma once

#include <dxgi1_6.h>
#include <string>
#include <wrl.h>
#include <d3d12.h>
#include "../Debug/Throw.h"

using Microsoft::WRL::ComPtr;

class GraphicsAdapterOutput {
public:
	GraphicsAdapterOutput() { }
	GraphicsAdapterOutput(const ComPtr<ID3D12Device> &device, const ComPtr<IDXGIAdapter> &adapter);
	GraphicsAdapterOutput(const ComPtr<ID3D12Device> &device, const ComPtr<IDXGIAdapter> &adapter, UINT adapter_output);
	GraphicsAdapterOutput(const ComPtr<ID3D12Device> &device, const ComPtr<IDXGIAdapter> &adapter, const std::string &adapter_output);
	GraphicsAdapterOutput(const ComPtr<ID3D12Device> &device, const ComPtr<IDXGIOutput> &adapter_output);

	void operator=(const GraphicsAdapterOutput &adapter_output);

	void clean_up();

	DXGI_OUTPUT_DESC get_desc() { return desc; }
	DXGI_GAMMA_CONTROL get_gamma_control() { return gamma_control; }
	DXGI_GAMMA_CONTROL_CAPABILITIES get_gamma_control_capabilities() { return gamma_control_capabilities; }

	void set_gamma_control(const DXGI_GAMMA_CONTROL &gamma_control) {
		adapter_output->SetGammaControl(&gamma_control);
		this->gamma_control = gamma_control;
	}

	ComPtr<IDXGIOutput> adapter_output = nullptr;

private:
	DXGI_OUTPUT_DESC desc{};
	DXGI_GAMMA_CONTROL gamma_control{};
	DXGI_GAMMA_CONTROL_CAPABILITIES gamma_control_capabilities{};

	void set_info(const DXGI_OUTPUT_DESC &desc);
	void set_info();
};

