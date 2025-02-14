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

	GET DXGI_OUTPUT_DESC get_desc() { return desc; }
	GET DXGI_MODE_DESC get_mode_desc() { return mode_desc; }
	GET MONITORINFOEXA get_monitor_info() { return monitor_info; }
	GET DISPLAY_DEVICEA get_display_device() { return display_device; }
	GET DXGI_GAMMA_CONTROL get_gamma_control() { return gamma_control; }
	GET DXGI_GAMMA_CONTROL_CAPABILITIES get_gamma_control_capabilities() { return gamma_control_capabilities; }
	
	void set_gamma_control(const DXGI_GAMMA_CONTROL &gamma_control) {
		adapter_output->SetGammaControl(&gamma_control);
		this->gamma_control = gamma_control;
	}

	DXGI_MODE_DESC find_closest_display_mode_to_current();
	
	ComPtr<IDXGIOutput> adapter_output = nullptr;

private:
	DXGI_OUTPUT_DESC desc{};
	DXGI_GAMMA_CONTROL gamma_control{};
	DXGI_GAMMA_CONTROL_CAPABILITIES gamma_control_capabilities{};
	DXGI_MODE_DESC mode_desc{};
	MONITORINFOEXA monitor_info{};
	DISPLAY_DEVICEA display_device{};

	void set_info(const DXGI_OUTPUT_DESC &desc);
	void set_info();
};

