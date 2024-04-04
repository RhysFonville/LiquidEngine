#include "GraphicsAdapterOutput.h"

GraphicsAdapterOutput::GraphicsAdapterOutput(const ComPtr<ID3D12Device> &device, const ComPtr<IDXGIAdapter> &adapter) : GraphicsAdapterOutput{device, adapter, 0u} { }

GraphicsAdapterOutput::GraphicsAdapterOutput(const ComPtr<ID3D12Device> &device, const ComPtr<IDXGIAdapter> &adapter, UINT adapter_output) {
	if (adapter->EnumOutputs(adapter_output, this->adapter_output.GetAddressOf()) == DXGI_ERROR_NOT_FOUND) {
		throw std::exception("Adapter output (monitor) not found.");
	}

	//HPEW(this->adapter_output->TakeOwnership(device.Get(), true));
	set_info();
}

GraphicsAdapterOutput::GraphicsAdapterOutput(const ComPtr<ID3D12Device> &device, const ComPtr<IDXGIAdapter> &adapter, const std::string &adapter_output) {
	int output_index = 0;
	bool output_found = false;
	while (adapter->EnumOutputs(output_index, this->adapter_output.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
		DXGI_OUTPUT_DESC desc;
		HPEW(this->adapter_output->GetDesc(&desc));
		if (wcscmp(desc.DeviceName, string_to_wstring(adapter_output).c_str()) == 0) {
			set_info(desc);
			output_found = true;
			break;
		}

		output_index++;
	}

	if (!output_found) {
		throw std::exception("Adapter output (monitor) not found.");
	}

	//HPEW(this->adapter_output->TakeOwnership(device.Get(), true));
}

GraphicsAdapterOutput::GraphicsAdapterOutput(const ComPtr<ID3D12Device> &device, const ComPtr<IDXGIOutput> &adapter_output) : adapter_output(adapter_output) {
	//HPEW(this->adapter_output->TakeOwnership(device.Get(), true));
	set_info();
}

void GraphicsAdapterOutput::operator=(const GraphicsAdapterOutput &adapter_output) {
	//this->adapter_output->ReleaseOwnership();
	this->adapter_output = adapter_output.adapter_output;
	set_info();
}

void GraphicsAdapterOutput::clean_up() {
	//this->adapter_output->ReleaseOwnership();
	adapter_output.Reset();
}

void GraphicsAdapterOutput::find_closest_display_mode_to_current(DXGI_MODE_DESC* out_current_display_mode) {
	HMONITOR hMonitor = desc.Monitor;
	MONITORINFOEXA monitorInfo;
	monitorInfo.cbSize = sizeof(MONITORINFOEXA);
	GetMonitorInfoA(hMonitor, &monitorInfo);
	DEVMODEA devMode;
	devMode.dmSize = sizeof(DEVMODEA);
	devMode.dmDriverExtra = 0;
	EnumDisplaySettingsA(monitorInfo.szDevice, ENUM_CURRENT_SETTINGS, &devMode);

	DXGI_MODE_DESC current;
	current.Width = devMode.dmPelsWidth;
	current.Height = devMode.dmPelsHeight;
	bool useDefaultRefreshRate = 1 == devMode.dmDisplayFrequency || 0 == devMode.dmDisplayFrequency;
	current.RefreshRate.Numerator = useDefaultRefreshRate ? 0 : devMode.dmDisplayFrequency;
	current.RefreshRate.Denominator = useDefaultRefreshRate ? 0 : 1;
	current.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	current.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	current.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	adapter_output->FindClosestMatchingMode(&current, out_current_display_mode, NULL);
}

void GraphicsAdapterOutput::set_info(const DXGI_OUTPUT_DESC &desc) {
	this->desc = desc;
	//HPEW(this->adapter_output->GetGammaControl(&gamma_control));
	//HPEW(this->adapter_output->GetGammaControlCapabilities(&gamma_control_capabilities));
}

void GraphicsAdapterOutput::set_info() {
	DXGI_OUTPUT_DESC desc;
	HPEW(this->adapter_output->GetDesc(&desc));
	set_info(desc);
}
