#pragma once

#include <filesystem>
#include <dxgi1_6.h>
#include <dxgidebug.h>
#include "GraphicsScene.h"
#include "../EditorGUI.h"

/**
 * Main class for rendering.
 */
class Renderer {
public:
	Renderer() { }
	Renderer(HWND window); // initializes direct3d 12

	void tick(float dt); // Updates pipeline and renders
	void clean_up(); // release com ojects and clean up memory

	void update(float dt); // update the direct3d pipeline (update command lists)
	void render(); // execute the command list
	void increment_fence(); // increment fences
	void wait_for_fence_cpu(); // wait for fences incrementation on cpu side
	void wait_for_fence_gpu(); // wait for fences incrementation on gpu side
	void flush_gpu();
	void wait_for_previous_frame(); // wait for command list completion

	void execute_command_list();

	void compile();

	void set_fullscreen(bool fullscreen);
	void toggle_fullscreen();
	GET bool is_fullscreen() const noexcept;

	GET UVector2 get_resolution() const noexcept;
	void set_resolution(const UVector2 &resolution, bool stretch = true);

	GraphicsScene scene;

private:
	friend class Window;

	ComPtr<ID3D12Device2> device = nullptr;
	ComPtr<ID3D12CommandQueue> command_queue = nullptr;
	ComPtr<IDXGISwapChain4> swap_chain = nullptr;
	ComPtr<ID3D12CommandAllocator> command_allocators[NUMBER_OF_BUFFERS] = { };
	ComPtr<ID3D12GraphicsCommandList> command_list = nullptr;
	ComPtr<ID3D12Fence> fences[NUMBER_OF_BUFFERS] = { };
	HANDLE fence_event = nullptr;
	ULONGLONG fence_values[NUMBER_OF_BUFFERS] = { };
	ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap = nullptr;
	UINT rtv_descriptor_size = 0u; // size of the rtv descriptor on the device (all front and back buffers will be the same size)
	ComPtr<ID3D12Resource> render_targets[NUMBER_OF_BUFFERS] = { };
	unsigned int buffer_index = 0u;
	ComPtr<IDXGIFactory4> factory = nullptr;
	std::string video_card_desc;
	ComPtr<IDXGIAdapter1> adapter = nullptr;
	ComPtr<IDXGIOutput> adapter_output = nullptr;
	ComPtr<ID3D12Resource> depth_stencil_buffer = nullptr; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
	ComPtr<ID3D12DescriptorHeap> depth_stencil_descriptor_heap = nullptr; // This is a heap for our depth/stencil buffer descriptor
	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	ComPtr<IDXGIDebug1> dxgi_debug = nullptr;
	ComPtr<ID3D12Debug> debug_interface = nullptr;
	ComPtr<ID3D12DebugDevice> debug_device = nullptr;
	ComPtr<ID3D12DebugCommandList> debug_command_list = nullptr;
	ComPtr<ID3D12DebugCommandQueue> debug_command_queue = nullptr;

	GraphicsDescriptorHeaps descriptor_heaps;

	void create_adapter_and_device();
	void create_command_queue();
	void create_swap_chain();
	void create_back_buffers_and_rtv_with_descriptor_heap();
	void create_command_allocators();
	void create_command_list();
	void create_fences_and_fence_event();
	void create_depth_stencil();
	void create_descriptor_heaps();

	UINT frame_index = 0u;

	DXGI_SAMPLE_DESC sample_desc = { };
	DXGI_MODE_DESC back_buffer_desc = { }; // this is to describe our display mode

	HWND window = nullptr;

	bool fullscreen = false;

	UVector2 resolution = UVector2(1920, 1080);

	FColor background_color = { 0.25f, 0.25f, 0.25f, 1.0f };
};
