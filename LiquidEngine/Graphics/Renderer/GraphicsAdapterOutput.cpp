#include "GraphicsAdapterOutput.h"

GraphicsAdapterOutput::GraphicsAdapterOutput(const ComPtr<ID3D12Device> &device, const ComPtr<IDXGIAdapter> &adapter) : GraphicsAdapterOutput{device, adapter, 0u} { }

GraphicsAdapterOutput::GraphicsAdapterOutput(const ComPtr<ID3D12Device> &device, const ComPtr<IDXGIAdapter> &adapter, UINT adapter_output) {
	if (adapter->EnumOutputs(adapter_output, this->adapter_output.GetAddressOf()) == DXGI_ERROR_NOT_FOUND) {
		throw std::exception("Adapter output (monitor) not found.");
	}

	//HPEW(this->adapter_output->TakeOwnership(device.Get(), true));
	this->adapter_output->GetDesc(&this->desc);
	set_info();
}

GraphicsAdapterOutput::GraphicsAdapterOutput(const ComPtr<ID3D12Device> &device, const ComPtr<IDXGIAdapter> &adapter, const std::string &adapter_output) {
	int output_index = 0;
	bool output_found = false;
	while (adapter->EnumOutputs(output_index, this->adapter_output.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
		DXGI_OUTPUT_DESC desc;
		HPEW(this->adapter_output->GetDesc(&desc));
		if (wcscmp(desc.DeviceName, string_to_wstring(adapter_output).c_str()) == 0) {
			this->desc = desc;
			set_info();
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
	this->adapter_output->GetDesc(&this->desc);
	set_info();
}

void GraphicsAdapterOutput::clean_up() {
	//this->adapter_output->ReleaseOwnership();
	adapter_output.Reset();
}

DXGI_MODE_DESC GraphicsAdapterOutput::find_closest_display_mode_to_current() {
	DXGI_MODE_DESC desc{};
	adapter_output->FindClosestMatchingMode(&mode_desc, &desc, NULL);
	return desc;
}

void GraphicsAdapterOutput::set_info() {
	//HPEW(this->adapter_output->GetGammaControl(&gamma_control));
	//HPEW(this->adapter_output->GetGammaControlCapabilities(&gamma_control_capabilities));

	monitor_info.cbSize = sizeof(MONITORINFOEXA);
	GetMonitorInfoA(desc.Monitor, &monitor_info);
	DEVMODEA dev_mode{};
	dev_mode.dmSize = sizeof(DEVMODEA);
	dev_mode.dmDriverExtra = 0;
	EnumDisplaySettingsA(monitor_info.szDevice, ENUM_CURRENT_SETTINGS, &dev_mode);

	mode_desc.Width = dev_mode.dmPelsWidth;
	mode_desc.Height = dev_mode.dmPelsHeight;

	bool use_default = 1 == dev_mode.dmDisplayFrequency || 0 == dev_mode.dmDisplayFrequency;
	mode_desc.RefreshRate.Numerator = use_default ? 0 : dev_mode.dmDisplayFrequency;
	mode_desc.RefreshRate.Denominator = use_default ? 0 : 1;

	mode_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	mode_desc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED;
	mode_desc.Scaling = DXGI_MODE_SCALING_UNSPECIFIED;

	display_device.cb = sizeof(DISPLAY_DEVICEA);
	EnumDisplayDevicesA(monitor_info.szDevice, 0, &display_device, 0);
}
