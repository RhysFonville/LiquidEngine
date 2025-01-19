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

	/**
	 * Initializes renderer.
	 * 
	 * \param window Window to pair the render to.
	 * \param exclude Objects to exclude from creation. Add the integers to the vector that correlate to the object to wish to not create.
	 * The integers correlate as follows:
	 * 0: Factory
	 * 1: Adapter
	 * 2: Adapter output
	 * 3: Device
	 * 4: Command queue
	 * 5: Swap chain
	 * 6: Render target view desciptor heap
	 * 7: Render target views
	 * 8: Command allocators
	 * 9: Command list
	 * 10: Fences and fence event
	 * 11: Depth stencil
	 * 12: Descriptor heaps
	 * 13: Blend state
	 * 14: Viewport and scissor rect
	 */
	void init_renderer(HWND window, std::vector<int> exclude = {});

	/** Calls render and present. */
	void tick(float dt);

	/** Releases DirectX all interfaces. */
	void clean_up();
	
	/**
	* Cleans renderer and re-initializes it. Descriptors are automatically filled into descriptor heaps.
	* 
	* \param exclude Vector to be passed into init_renderer and the exclusions.
	*/
	
	void refresh(std::vector<int> exclude = {}) { clean_up(); init_renderer(window, exclude); refill_descriptor_heap(); }

	/**
	* Fills command list and calls pipelines.
	* 
	* \param dt Delta time.
	*/
	void render(float dt);
	
	/** Executes swap chain. Presents and increments fence. */
	void present();
	void wait_for_previous_frame(); // wait for fences incrementation on cpu side
	void wait_for_fence_gpu(); // wait for fences incrementation on gpu side
	void flush_gpu();
	void execute_command_list();
	void signal();
	void set_frame_index();

	/**
	 * Compiles scene and pipelines.
	 * 
	 * \param compile_components If components are already compiled, keep false. You may set to true if the renderer was just cleaned.
	 * \param compile_scene If graphics scene components are already compiled, keep false. You may set to true if the renderer was just cleaned.
	 */
	void compile();

	/**
	 * Resizes swap chain, viewport/scissor rect, and depth stencil. Automatically called when window resizes.
	 * 
	 * \param size Size of new buffer sizes.
	 */
	void resize(const UVector2 &size);

	void set_msaa_sample_count(UINT count);

	void set_fullscreen(bool fullscreen);
	void toggle_fullscreen();
	GET bool is_fullscreen() const noexcept;

	GET UVector2 get_resolution() const noexcept;
	void set_resolution(const UVector2 &resolution);

	GraphicsScene scene;

	bool vsync{false};
	bool restrict_present_to_adapter_output{false};
	
	/** Clear the render target at the beginning each frame. If false, the frame will be cleared with the background color. */
	bool clear_render_target{true};

private:
	friend class Window;

	ComPtr<ID3D12Device> device{nullptr};
	ComPtr<IDXGISwapChain3> swap_chain{nullptr};

	ComPtr<ID3D12CommandQueue> command_queue{nullptr};
	ComPtr<ID3D12CommandAllocator> command_allocator{nullptr};
	ComPtr<ID3D12GraphicsCommandList> command_list{nullptr};

	ComPtr<ID3D12Fence> fence{nullptr};
	HANDLE fence_event{nullptr};
	ULONGLONG fence_value{};

	ComPtr<ID3D12DescriptorHeap> rtv_descriptor_heap{nullptr};
	UINT rtv_descriptor_size = 0u; // size of the rtv descriptor on the device (all front and back buffers will be the same size)
	ComPtr<ID3D12Resource> render_targets[NUMBER_OF_BUFFERS]{};
	ComPtr<ID3D12Resource> msaa_render_targets[NUMBER_OF_BUFFERS]{};

	ComPtr<IDXGIFactory2> factory{nullptr};

	GraphicsAdapter adapter{};
	GraphicsAdapterOutput adapter_output{};

	ComPtr<ID3D12Resource> depth_stencil_buffer{nullptr}; // This is the memory for our depth buffer. it will also be used for a stencil buffer in a later tutorial
	ComPtr<ID3D12DescriptorHeap> depth_stencil_descriptor_heap{nullptr}; // This is a heap for our depth/stencil buffer descriptor

	ComPtr<IDXGIDebug1> dxgi_debug{nullptr};
	ComPtr<ID3D12Debug> debug_interface{nullptr};
	ComPtr<ID3D12DebugDevice> debug_device{nullptr};
	ComPtr<ID3D12DebugCommandList> debug_command_list{nullptr};
	ComPtr<ID3D12DebugCommandQueue> debug_command_queue{nullptr};
	ComPtr<ID3D12InfoQueue> info_queue{nullptr};

	GraphicsResourceDescriptorHeap descriptor_heap{};

	void create_factory();
	void create_device();
	void create_command_queue();
	void create_swap_chain();
	void create_rtv_descriptor_heap();
	void create_rtvs();
	void create_command_allocators();
	void create_command_list();
	void create_fences_and_fence_event();
	void create_depth_stencil(const UVector2 &size);
	void create_descriptor_heap();
	void set_blend_state();
	void set_viewport_and_scissor_rect(const UVector2 &size);

	void refill_descriptor_heap();

	void setup_imgui_section();

	UINT frame_index = 0u;

	DXGI_SAMPLE_DESC msaa_sample_desc{8u, 0u};
	const DXGI_SAMPLE_DESC sample_desc{1u, 0u};
	D3D12_BLEND_DESC blend_desc{};

	D3D12_VIEWPORT viewport{};
	D3D12_RECT scissor_rect{};

	HWND window{nullptr};

	bool fullscreen = false;

	UVector2 resolution = UVector2(1920u, 1080u);

	FColor background_color = { 0.25f, 0.25f, 0.25f, 1.0f };

	bool skip_frame = false;
};
