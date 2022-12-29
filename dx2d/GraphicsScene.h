#pragma once

#include <DirectXMath.h>
#include <filesystem>
#include <dxgi1_4.h>
#include "d3dx12.h"
#include "Object.h"
#include "globalutil.h"
#include "CameraComponent.h"
#include "DirectionalLightComponent.h"
#include "PointLightComponent.h"
#include "SpotlightComponent.h"

#pragma comment(lib,"D3D12.lib")
#pragma comment (lib, "D3DCompiler.lib")
#pragma comment(lib, "dxgi.lib")

static constexpr UINT MAX_LIGHTS_PER_TYPE = 16u;

static constexpr UINT NUMBER_OF_BUFFERS = 2u;

using Microsoft::WRL::ComPtr;



class GraphicsScene {
public:
	GraphicsScene() {}
	GraphicsScene(HWND window,
		const ObjectVector &objects);

	void compile();

	void tick();

	/*GET D3D11_PRIMITIVE_TOPOLOGY get_primitive_topology() const noexcept;
	void set_primitive_topology(D3D11_PRIMITIVE_TOPOLOGY primitive_topology) noexcept;

	GET Color get_background_color() const noexcept;
	void set_background_color(Color background_color) noexcept;

	GET D3D11_VIEWPORT get_viewport() const noexcept;
	void set_viewport(D3D11_VIEWPORT viewport) noexcept;*/

	GET std::shared_ptr<CameraComponent> camera() const;

	GET std::vector<std::shared_ptr<DirectionalLightComponent>> directional_lights() const;
	GET std::vector<std::shared_ptr<PointLightComponent>> point_lights() const;
	GET std::vector<std::shared_ptr<SpotlightComponent>> spotlights() const;

private:
	friend class Window;
	friend class Texture;

	GET std::shared_ptr<MeshComponent> obj_mesh(const Object &object) const;

	void create_essentials();

	bool vsync_enabled = false;
	ComPtr<ID3D12Device> device = nullptr;
	ComPtr<ID3D12CommandQueue> command_queue = nullptr;
	std::string video_card_description;
	ComPtr<IDXGISwapChain3> swap_chain = nullptr;
	ComPtr<ID3D12DescriptorHeap> render_target_view_heap = nullptr;
	ComPtr<ID3D12Resource> back_buffer_render_target[2] = { nullptr, nullptr };
	unsigned int buffer_index = 0u;
	ComPtr<ID3D12CommandAllocator> command_allocator = nullptr;
	ComPtr<ID3D12GraphicsCommandList> command_list = nullptr;
	ComPtr<ID3D12PipelineState> pipeline_state = nullptr;
	ComPtr<ID3D12Fence> fence = nullptr;
	HANDLE fence_event = nullptr;
	ULONGLONG fence_value = 0ll;
	ComPtr<IDXGIFactory4> factory = nullptr;
	ComPtr<IDXGIAdapter> adapter = nullptr;
	ComPtr<IDXGIOutput> adapter_output = nullptr;
	ComPtr<ID3D12PipelineState> pipeline_state_object = nullptr; // pso containing a pipeline state
	ComPtr<ID3D12RootSignature> root_signature = nullptr; // root signature defines data shaders will access
	
	ComPtr<ID3D12Debug> debug_interface = nullptr;
	ComPtr<ID3D12DebugDevice> debug_device = nullptr;
	ComPtr<ID3D12DebugCommandList> debug_command_list = nullptr;
	ComPtr<ID3D12DebugCommandQueue> debug_command_queue = nullptr;

	D3D12_VIEWPORT viewport = { }; // area that output from rasterizer will be stretched to.
	D3D12_RECT scissor_rect = { }; // the area to draw in. pixels outside that area will not be drawn onto

	ComPtr<ID3D12Resource> vertex_buffer = { }; // a default buffer in GPU memory that we will load vertex data for our triangle into

	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view = { }; // a structure containing a pointer to the vertex data in gpu memory
											   // the total size of the buffer, and the size of each element (vertex)

	DXGI_SAMPLE_DESC sample_description = { };

	HWND window = nullptr;

	ObjectVector objects = { };

	UVector2 resolution = UVector2(3840, 2160);

	FColor background_color = { 0.25f, 0.25f, 0.25f };
};
