#pragma once

#include <filesystem>
#include "Object.h"
#include "CameraComponent.h"
#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "SpotlightComponent.h"
#include "GraphicsPipeline.h"
#include "AppearanceComponent.h"

#pragma comment(lib,"D3D12.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment(lib, "dxgi.lib")

_declspec(align(256u))
struct PerFrameVSCB { // b0
	XMMATRIX WVP;
};

_declspec(align(256u))
struct PerObjectVSCB { // b1
	XMMATRIX transform;
};

static constexpr UINT MAX_LIGHTS_PER_TYPE = 16u;

_declspec(align(256u))
struct PerFramePSCB { // b2
	DirectionalLightComponent::ConstantBufferStruct directional_lights[MAX_LIGHTS_PER_TYPE] = { };
	PointLightComponent::ConstantBufferStruct point_lights[MAX_LIGHTS_PER_TYPE] = { };
	SpotlightComponent::ConstantBufferStruct spotlights[MAX_LIGHTS_PER_TYPE] = { };
	uint directional_light_count = 0;
	uint point_light_count = 0;
	uint spotlight_count = 0;

	XMFLOAT3 camera_position;
};

class GraphicsScene {
public:
	GraphicsScene() { }
	GraphicsScene(HWND window, const std::vector<std::shared_ptr<AppearanceComponent>> &appearances = { }); // initializes direct3d 12

	void tick(); // Updates pipeline and renders
	void clean_up(); // release com ojects and clean up memory

	void update(); // update the direct3d pipeline (update command lists)
	void render(); // execute the command list
	void increment_fence(); // increment fences
	void wait_for_fence_cpu(); // wait for fences incrementation on cpu side
	void wait_for_fence_gpu(); // wait for fences incrementation on gpu side
	void wait_for_previous_frame(); // wait for command list completion

	void compile();

	void set_fullscreen(bool fullscreen);
	void toggle_fullscreen();
	GET bool is_fullscreen() const noexcept;

	GET UVector2 get_resolution() const noexcept;
	void set_resolution(const UVector2 &resolution, bool reset_om_viewing_settings = true);

	GET float get_delta_time() const noexcept;

	std::vector<std::shared_ptr<AppearanceComponent>> appearances;

	std::shared_ptr<CameraComponent> camera;

private:
	friend class Window;

	ComPtr<ID3D12Device> device = nullptr;
	ComPtr<ID3D12CommandQueue> command_queue = nullptr;
	ComPtr<IDXGISwapChain3> swap_chain = nullptr;
	ComPtr<ID3D12CommandAllocator> command_allocators[GraphicsPipeline::NUMBER_OF_BUFFERS] = { };
	ComPtr<ID3D12GraphicsCommandList> command_list = nullptr;
	ComPtr<ID3D12Fence> fences[GraphicsPipeline::NUMBER_OF_BUFFERS] = { };
	HANDLE fences_event = nullptr;
	ULONGLONG fence_values[GraphicsPipeline::NUMBER_OF_BUFFERS] = { };
	ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap = nullptr;
	UINT rtv_descriptor_size = 0u; // size of the rtv descriptor on the device (all front and back buffers will be the same size)
	ComPtr<ID3D12Resource> render_targets[GraphicsPipeline::NUMBER_OF_BUFFERS] = { };
	unsigned int buffer_index = 0u;
	ComPtr<IDXGIFactory4> factory = nullptr;
	std::string video_card_desc;
	ComPtr<IDXGIAdapter1> adapter = nullptr;
	ComPtr<IDXGIOutput> adapter_output = nullptr;

	ComPtr<ID3D12Debug> debug_interface = nullptr;
	ComPtr<ID3D12DebugDevice> debug_device = nullptr;
	ComPtr<ID3D12DebugCommandList> debug_command_list = nullptr;
	ComPtr<ID3D12DebugCommandQueue> debug_command_queue = nullptr;

	void create_adapter_and_device();
	void create_command_queue();
	void create_swap_chain();
	void create_back_buffers_and_rtv_with_descriptor_heap();
	void create_command_allocators();
	void create_command_list();
	void create_fences_and_fences_event();

	template <typename T>
	class ConstantBuffer {
	public:
		ConstantBuffer(const T &obj)
			: obj(std::make_shared<T>(obj)) {
			cb = GraphicsPipeline::RootSignature::ConstantBuffer(*this->obj);
		}

		std::shared_ptr<T> obj;
		GraphicsPipeline::RootSignature::ConstantBuffer cb;
	};

	struct CBS {
		ConstantBuffer<PerFrameVSCB> per_frame_vs = ConstantBuffer<PerFrameVSCB>(PerFrameVSCB());
		ConstantBuffer<PerObjectVSCB> per_object_vs = ConstantBuffer<PerObjectVSCB>(PerObjectVSCB());
		//ConstantBuffer<PerFramePSCB> per_frame_ps = ConstantBuffer<PerFramePSCB>(PerFramePSCB());
	} cbs;

	UINT frame_index = 0u;

	DXGI_SAMPLE_DESC sample_desc = { };
	DXGI_MODE_DESC back_buffer_desc = { }; // this is to describe our display mode

	HWND window = nullptr;

	bool fullscreen = false;

	UVector2 resolution = UVector2(3840, 2160);

	FColor background_color = { 0.25f, 0.25f, 0.25f, 1.0f };

	struct DeltaTime {
		float dt = 0.0f;
		std::chrono::system_clock::time_point tp1 = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point tp2 = tp1;
	} dt;
};
