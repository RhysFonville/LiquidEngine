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

using Microsoft::WRL::ComPtr;

class GraphicsScene {
public:
	GraphicsScene() {}
	GraphicsScene(HWND window, const ObjectVector &objects);

	void initD3D(); // initializes direct3d 12
	void update_pipeline(); // update the direct3d pipeline (update command lists)
	void render(); // execute the command list
	void tick(); // Updates pipeline and renders
	void cleanup(); // release com ojects and clean up memory
	void wait_for_previous_frame(); // wait until gpu is finished with command list

	GET std::shared_ptr<CameraComponent> camera() const;

	GET std::vector<std::shared_ptr<DirectionalLightComponent>> directional_lights() const;
	GET std::vector<std::shared_ptr<PointLightComponent>> point_lights() const;
	GET std::vector<std::shared_ptr<SpotlightComponent>> spotlights() const;

	FColor background_color = { 0.25f, 0.25f, 0.25f, 1.0f };

private:
	friend class Window;
	friend class Texture;

	GET std::shared_ptr<MeshComponent> obj_mesh(const Object &object) const;

	ObjectVector objects = { };


	void create_adapter_and_device();
	void create_command_queue();
	void create_swap_chain();
	void create_back_buffers_and_rtv_with_descriptor_heap();
	void create_command_allocators();
	void create_command_list();
	void create_fence_and_fence_event();
	void create_root_signature();
	void create_vertex_and_pixel_shaders();
	void create_pso();
	void create_depth_stencil();
	void create_vertex_buffer();
	void fill_out_om_viewing_settings();

	static constexpr UINT MAX_LIGHTS_PER_TYPE = 16u;

	static constexpr UINT NUMBER_OF_BUFFERS = 3u;

	ComPtr<ID3D12Device> device = nullptr;
	ComPtr<ID3D12CommandQueue> command_queue = nullptr;
	ComPtr<IDXGISwapChain3> swap_chain = nullptr;
	ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap = nullptr;
	UINT rtv_descriptor_size = 0u; // size of the rtv descriptor on the device (all front and back buffers will be the same size)
	ComPtr<ID3D12Resource> render_targets[NUMBER_OF_BUFFERS] = { };
	unsigned int buffer_index = 0u;
	ComPtr<ID3D12CommandAllocator> command_allocator[NUMBER_OF_BUFFERS] = { };
	ComPtr<ID3D12GraphicsCommandList> command_list = nullptr;
	ComPtr<ID3D12Fence> fence[NUMBER_OF_BUFFERS] = { };
	HANDLE fence_event = nullptr;
	ULONGLONG fence_value[NUMBER_OF_BUFFERS] = { };
	ComPtr<IDXGIFactory4> factory = nullptr;
	std::string video_card_description;
	ComPtr<IDXGIAdapter1> adapter = nullptr;
	ComPtr<IDXGIOutput> adapter_output = nullptr;
	
	ComPtr<ID3D12PipelineState> pipeline_state_object = nullptr; // pso containing a pipeline state
	ComPtr<ID3D12RootSignature> root_signature = nullptr; // root signature defines data shaders will access
	
	ComPtr<ID3D12Resource> vertex_buffer = nullptr; // a default buffer in GPU memory that we will load vertex data for our triangle into
	D3D12_VERTEX_BUFFER_VIEW vertex_buffer_view; // a structure containing a pointer to the vertex data in gpu memory
											   // the total size of the buffer, and the size of each element (vertex)
	ComPtr<ID3D12Resource> vertex_buffer_upload_heap = nullptr;

	ComPtr<ID3D12Resource> depth_stencil_buffer; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
	ComPtr<ID3D12DescriptorHeap> depth_stencil_descriptor_heap; // This is a heap for our depth/stencil buffer descriptor

	ComPtr<ID3D12Debug> debug_interface = nullptr;
	ComPtr<ID3D12DebugDevice> debug_device = nullptr;
	ComPtr<ID3D12DebugCommandList> debug_command_list = nullptr;
	ComPtr<ID3D12DebugCommandQueue> debug_command_queue = nullptr;

	D3D12_VIEWPORT viewport = { }; // area that output from rasterizer will be stretched to.
	D3D12_RECT scissor_rect = { }; // the area to draw in. pixels outside that area will not be drawn onto

	DXGI_SAMPLE_DESC sample_desc = {};

	D3D12_SHADER_BYTECODE vs_bytecode = { };
	D3D12_SHADER_BYTECODE hs_bytecode = { };
	D3D12_SHADER_BYTECODE ds_bytecode = { };
	D3D12_SHADER_BYTECODE gs_bytecode = { };
	D3D12_SHADER_BYTECODE ps_bytecode = { };

	UINT frame_index = 0u;

	DXGI_SAMPLE_DESC sample_description = { };

	HWND window = nullptr;

	bool fullscreen = false;

	UVector2 resolution = UVector2(3840, 2160);
};
