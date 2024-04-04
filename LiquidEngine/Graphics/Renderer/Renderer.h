#pragma once

#include <filesystem>
#include <dxgidebug.h>
#include <CommonStates.h>
#include "../GraphicsScene.h"
#include "../../EditorGUI.h"
#include "GraphicsAdapter.h"
#include "GraphicsAdapterOutput.h"

/**
 * Main class for rendering. Holds DirectX interfaces and executes graphics pipelines
 * \see GraphicsPipeline
 */
class Renderer {
public:
	Renderer() { }
	Renderer(HWND window); // initializes direct3d 12

	void init_renderer(HWND window);

	/** Calls render and present. */
	void tick(float dt);

	/** Releases DirectX all interfaces. Cleans graphics scene. */
	void clean_up();
	
	/**
	* Fills command list and calls pipelines.
	* 
	* \param dt Delta time.
	*/
	void render(float dt);
	
	/** Executes swap chain. Presents and increments fence. */
	void present();
	void increment_fence(); // increment fences
	void wait_for_fence_cpu(); // wait for fences incrementation on cpu side
	void wait_for_fence_gpu(); // wait for fences incrementation on gpu side
	void flush_gpu();
	void wait_for_previous_frame(); // wait for command list completion

	void execute_command_list();

	/**
	 * Compiles scene and pipelines.
	 * 
	 * \param compile_components If mesh components are already compiled, keep false. Setting to true if mesh components are already compiled, it is unneccesary. You may set to true if the renderer was just cleaned.
	 */
	void compile(bool compile_components = false);

	/**
	 * Resizes swap chain, viewport/scissor rect, and depth stencil. Automatically called when window resizes.
	 * 
	 * \param size Size of new buffer sizes.
	 */
	void resize(const UVector2 &size);

	void set_fullscreen(bool fullscreen);
	void toggle_fullscreen();
	GET bool is_fullscreen() const noexcept;

	GET UVector2 get_resolution() const noexcept;
	void set_resolution(const UVector2 &resolution);

	GraphicsScene scene;

	bool vsync{false};
	bool restrict_present_to_adapter_output{false};

private:
	friend class Window;

	ComPtr<ID3D12Device> device = nullptr;
	ComPtr<IDXGISwapChain3> swap_chain = nullptr;

	ComPtr<ID3D12CommandQueue> command_queue = nullptr;
	ComPtr<ID3D12CommandAllocator> command_allocators[NUMBER_OF_BUFFERS] = { };
	ComPtr<ID3D12GraphicsCommandList> command_list = nullptr;

	ComPtr<ID3D12Fence> fences[NUMBER_OF_BUFFERS] = { };
	HANDLE fence_event = nullptr;
	ULONGLONG fence_values[NUMBER_OF_BUFFERS] = { };

	ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap = nullptr;
	UINT rtv_descriptor_size = 0u; // size of the rtv descriptor on the device (all front and back buffers will be the same size)
	ComPtr<ID3D12Resource> render_targets[NUMBER_OF_BUFFERS] = { };

	ComPtr<IDXGIFactory2> factory = nullptr;

	GraphicsAdapter adapter{};
	GraphicsAdapterOutput adapter_output{};

	ComPtr<ID3D12Resource> depth_stencil_buffer = nullptr; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
	ComPtr<ID3D12DescriptorHeap> depth_stencil_descriptor_heap = nullptr; // This is a heap for our depth/stencil buffer descriptor
	D3D12_DEPTH_STENCIL_DESC depth_stencil_desc = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);

	ComPtr<IDXGIDebug1> dxgi_debug = nullptr;
	ComPtr<ID3D12Debug> debug_interface = nullptr;
	ComPtr<ID3D12DebugDevice> debug_device = nullptr;
	ComPtr<ID3D12DebugCommandList> debug_command_list = nullptr;
	ComPtr<ID3D12DebugCommandQueue> debug_command_queue = nullptr;
	ComPtr<ID3D12InfoQueue> info_queue = nullptr;

	GraphicsDescriptorHeaps descriptor_heaps;

	void create_device();
	void create_command_queue();
	void create_swap_chain();
	void create_descriptor_heap();
	void create_rtvs();
	void create_command_allocators();
	void create_command_list();
	void create_fences_and_fence_event();
	void create_depth_stencil(const UVector2 &size);
	void create_descriptor_heaps();
	void set_blend_state();
	void set_viewport_and_scissor_rect(const UVector2 &size);

	void setup_imgui_section();

	UINT frame_index = 0u;

	DXGI_SAMPLE_DESC sample_desc{};
	D3D12_BLEND_DESC blend_desc{};

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissor_rect{};

	HWND window = nullptr;

	bool fullscreen = false;

	UVector2 resolution = UVector2(1920u, 1080u);

	FColor background_color = { 0.25f, 0.25f, 0.25f, 1.0f };

	bool skip_frame = false;
};
