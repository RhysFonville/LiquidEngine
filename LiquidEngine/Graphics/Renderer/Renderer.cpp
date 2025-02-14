#include "Renderer.h"

Renderer::Renderer(HWND window) {
	init_renderer(window);
}

void Renderer::init_renderer(HWND window) {
	this->window = window;

	HPEW(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface)));
	debug_interface->EnableDebugLayer();
	//debug_interface->SetEnableGPUBasedValidation(true);

	DXGIGetDebugInterface1(NULL, IID_PPV_ARGS(&dxgi_debug));
	dxgi_debug->EnableLeakTrackingForThread();

	create_factory();
	adapter = GraphicsAdapter{factory};

	create_device();
	adapter_output = GraphicsAdapterOutput{device, adapter.adapter};
	
	auto mode_desc{adapter_output.get_mode_desc()};
	resolution = UVector2{mode_desc.Width, mode_desc.Height};

	UVector2 client_size{get_client_size(window)};

	create_command_queue();
	create_swap_chain();
	create_rtv_descriptor_heap();
	create_rtvs(client_size);
	create_command_allocators();
	create_command_list();
	create_fences_and_fence_event();
	create_depth_stencil(client_size);
	create_descriptor_heap();
	set_blend_state();
	set_viewport_and_scissor_rect(client_size);

	HPEW(device->QueryInterface(IID_PPV_ARGS(&debug_device)));
	HPEW(device->QueryInterface(IID_PPV_ARGS(&info_queue)));
	HPEW(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true));
	HPEW(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true));
	HPEW(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true));

	EditorGUI::init_with_renderer(window, device.Get(), NUMBER_OF_BUFFERS, msaa_sample_desc, descriptor_heap.get().Get());
	descriptor_heap.reserve_descriptor_index(0u);
}

void Renderer::create_factory() {
	HPEW(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)));
}

void Renderer::create_device() {
	HPEW(D3D12CreateDevice(adapter.adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));
	device->SetName(L"Main Device");
}

void Renderer::create_command_queue() {
	const D3D12_COMMAND_QUEUE_DESC desc = {
		.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
		.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		.NodeMask = 0,
	};

	HPEW(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&command_queue)));
	HPEW(command_queue->SetName(L"Main Command Queue"));

	HPEW(command_queue->QueryInterface(IID_PPV_ARGS(&debug_command_queue)));
}

void Renderer::create_swap_chain() {
	const DXGI_SWAP_CHAIN_DESC1 swap_chain_desc{
		.Width = 0u,
		.Height = 0u,
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.Stereo = false,
		.SampleDesc = sample_desc,
		.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT,
		.BufferCount = NUMBER_OF_BUFFERS,
		.Scaling = DXGI_SCALING_STRETCH,
		.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD,
		.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED,
		.Flags = NULL
	};

	DXGI_MODE_DESC mode_desc{adapter_output.get_mode_desc()};
	const DXGI_SWAP_CHAIN_FULLSCREEN_DESC fullscreen_desc{
		.RefreshRate = mode_desc.RefreshRate,
		.ScanlineOrdering = mode_desc.ScanlineOrdering,
		.Scaling = mode_desc.Scaling,
		.Windowed = !fullscreen
	};

	IDXGISwapChain1* swap_chain1;
	HPEW(factory->CreateSwapChainForHwnd(
		command_queue.Get(),
		window,
		&swap_chain_desc,
		&fullscreen_desc,
		adapter_output.adapter_output.Get(),
		&swap_chain1
	));
	HPEW(swap_chain1->QueryInterface(IID_PPV_ARGS(&swap_chain)));
	swap_chain1->Release();

	frame_index = swap_chain->GetCurrentBackBufferIndex();
}

void Renderer::create_rtv_descriptor_heap() {
	// describe an rtv descriptor heap and create
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
	rtv_heap_desc.NumDescriptors = NUMBER_OF_BUFFERS*2u;
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HPEW(device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtv_descriptor_heap)));

	rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}
	
void Renderer::create_rtvs(const UVector2& size) {
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart());
	
	// Create a RTV for each buffer
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		HPEW(swap_chain->GetBuffer(i, IID_PPV_ARGS(&render_targets[i])));

		device->CreateRenderTargetView(render_targets[i].Get(), nullptr, rtv_handle);
		HPEW(render_targets[i]->SetName(string_to_wstring("Render Target #" + std::to_string(i)).c_str()));

		rtv_handle.Offset(1, rtv_descriptor_size);
	}

	D3D12_FEATURE_DATA_MULTISAMPLE_QUALITY_LEVELS ms_levels{
		.Format = DXGI_FORMAT_R8G8B8A8_UNORM,
		.SampleCount = msaa_sample_desc.Count,
		.Flags = D3D12_MULTISAMPLE_QUALITY_LEVELS_FLAG_NONE
	};

	HPEW(device->CheckFeatureSupport(D3D12_FEATURE_MULTISAMPLE_QUALITY_LEVELS, &ms_levels, sizeof(ms_levels)));
	
	if (msaa_sample_desc.Quality > ms_levels.NumQualityLevels-1u) throw std::exception{"Invalid MSAA quality level."};
	
	D3D12_RESOURCE_DESC msaa_render_target_desc{
		.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		.Alignment = 0u,
		.Width = size.x,
		.Height = size.y,
		.DepthOrArraySize = 1,
		.MipLevels = 1u,
		.Format = ms_levels.Format,
		.SampleDesc = {msaa_sample_desc.Count, msaa_sample_desc.Quality},
		.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		.Flags = D3D12_RESOURCE_FLAG_ALLOW_RENDER_TARGET
	};

	D3D12_CLEAR_VALUE clear_value = {};
	clear_value.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	clear_value.Color[0] = background_color.r;
	clear_value.Color[1] = background_color.g;
	clear_value.Color[2] = background_color.b;
	clear_value.Color[3] = background_color.a;

	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		auto prop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		HPEW(device->CreateCommittedResource(
			&prop,
			D3D12_HEAP_FLAG_NONE,
			&msaa_render_target_desc,
			D3D12_RESOURCE_STATE_RESOLVE_SOURCE,
			&clear_value,
			IID_PPV_ARGS(msaa_render_targets[i].GetAddressOf())
		));

		device->CreateRenderTargetView(msaa_render_targets[i].Get(), nullptr, rtv_handle);
		HPEW(render_targets[i]->SetName(string_to_wstring("MSAA Render Target #" + std::to_string(i)).c_str()));
		
		rtv_handle.Offset(1, rtv_descriptor_size);
	}
}

void Renderer::create_rtvs() {
	create_rtvs(get_client_size(window));
}

void Renderer::create_command_allocators() {
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		HPEW(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator)));
		HPEW(command_allocator->SetName(string_to_wstring("Command Allocator #" + std::to_string(i)).c_str()));
	}
}

void Renderer::create_command_list() {
	HPEW(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator.Get(), NULL, IID_PPV_ARGS(&command_list)));
	HPEW(command_list->SetName(L"Main command list"));

	HPEW(command_list->QueryInterface(IID_PPV_ARGS(&debug_command_list)));

	command_list->Close();
}

void Renderer::create_fences_and_fence_event() {
	// create the fences
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		HPEW(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence)));
		HPEW(fence->SetName(string_to_wstring("Fence #" + std::to_string(i)).c_str()));

		fence_value = 0;
	}

	fence_event = CreateEventA(nullptr, false, false, nullptr);
	if (fence_event == nullptr) {
		throw std::exception("Failed to create fences event.");
	}
}

void Renderer::create_depth_stencil(const UVector2 &size) {
	D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
	dsv_heap_desc.NumDescriptors = 1;
	dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HPEW(device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(&depth_stencil_descriptor_heap)));
	HPEW(depth_stencil_descriptor_heap->SetName(L"Depth Stencil Descriptor Heap"));

	D3D12_DEPTH_STENCIL_VIEW_DESC depth_stencil_desc = {};
	depth_stencil_desc.Format = DXGI_FORMAT_D32_FLOAT;
	depth_stencil_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2DMS;
	depth_stencil_desc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depth_clear_value = {};
	depth_clear_value.Format = DXGI_FORMAT_D32_FLOAT;
	depth_clear_value.DepthStencil.Depth = 1.0f;
	depth_clear_value.DepthStencil.Stencil = 0;

	auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto tex = CD3DX12_RESOURCE_DESC::Tex2D(
		DXGI_FORMAT_D32_FLOAT,
		size.x, size.y, 1u, 1u,
		msaa_sample_desc.Count, msaa_sample_desc.Quality,
		D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL
	);
	
	HPEW(device->CreateCommittedResource(
		&heap_properties,
		D3D12_HEAP_FLAG_NONE,
		&tex,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depth_clear_value,
		IID_PPV_ARGS(&depth_stencil_buffer)
	));
	HPEW(depth_stencil_buffer->SetName(L"Depth Stencil Buffer"));

	device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_desc, depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart());
}

void Renderer::create_depth_stencil() {
	create_depth_stencil(get_client_size(window));
}

void Renderer::create_descriptor_heap() {
	descriptor_heap.compile(device);
}

void Renderer::set_blend_state() {
	blend_desc = CommonStates::AlphaBlend;
}

void Renderer::set_viewport_and_scissor_rect(const UVector2 &size) {
	// Fill out the Viewport
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)size.x;
	viewport.Height = (float)size.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	scissor_rect.left = 0;
	scissor_rect.top = 0;
	scissor_rect.right = size.x;
	scissor_rect.bottom = size.y;
}

void Renderer::set_viewport_and_scissor_rect() {
	set_viewport_and_scissor_rect(get_client_size(window));
}

void Renderer::refill_descriptor_heap() {
	for (auto &mesh : scene.static_meshes) {
		mesh->material.component->pipeline.root_signature.create_views(device, descriptor_heap);
	}
}

void Renderer::setup_imgui_section() {
	auto end_imgui = [&]() {
		ImGui::TreePop();
		ImGui::End();
		ImGui::Render();
	};

	if (ImGui::TreeNode("Renderer")) {
		ImGui::Checkbox("Clear render target", &clear_render_target);

		float color[4]{background_color.r, background_color.g, background_color.b, background_color.a};
		if (ImGui::InputFloat4("Background color", color)) {
			background_color = FColor{color[0], color[1], color[2], color[3]};
		}

		UINT count{msaa_sample_desc.Count};
		if (ImGui::InputInt("Sample count", (int*)&count, 2, 4, ImGuiInputTextFlags_EnterReturnsTrue)) {
			set_msaa_sample_count(count);

			end_imgui();
			skip_frame = true;
			return;
		}

		static int res[2]{(int)resolution.x, (int)resolution.y};
		ImGui::InputInt2("Resolution", res);
		if (ImGui::Button("Set resolution")) {
			set_resolution(UVector2{(UINT)res[0], (UINT)res[1]});
		}

		ImGui::Checkbox("V-sync", &vsync);
		
		if (ImGui::Checkbox("Fullscreen", &fullscreen)) {
			set_fullscreen(fullscreen);
		}

		{ // Adapter
			auto desc = adapter.get_desc();
			std::string name{wstring_to_string(desc.Description)};
			if (ImGui::InputText("Adapter", &name, ImGuiInputTextFlags_EnterReturnsTrue)) {
				//end_imgui();
				device.Reset();
				adapter_output.clean_up();
				adapter.clean_up();
				factory.Reset();

				create_factory();
				adapter = GraphicsAdapter{factory, name};
				create_device();
				adapter_output = GraphicsAdapterOutput{device, adapter.adapter};

				end_imgui();
				skip_frame = true;
				return;
			}
			ImGui::Text("Adapter information");
			auto mem_info{adapter.get_memory_info()};
			ImGui::Text(("Current video memory usage vs. budget: " +
				std::to_string(mem_info.CurrentUsage/1000000) + " MB / " +
				std::to_string(mem_info.Budget/1000000) + " MB " +
				std::format("({:.2f}%%)", (float)mem_info.CurrentUsage / (float)mem_info.Budget * 100.0f)
			).c_str());
		}

		ImGui::Spacing();

		{ // Adapter output
			auto desc = adapter_output.get_desc();
			auto mode_desc = adapter_output.get_mode_desc();
			std::string name{wstring_to_string(desc.DeviceName)};
			if (ImGui::InputText("Adapter output", &name, ImGuiInputTextFlags_EnterReturnsTrue)) {
				flush_gpu();

				bool fs = is_fullscreen();
				if (fs)
					set_fullscreen(false);

				swap_chain.Reset();
				for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
					render_targets[i].Reset();
				}

				adapter_output = GraphicsAdapterOutput{device, adapter.adapter, name};
				create_swap_chain();
				create_rtvs();

				if (fs)
					set_fullscreen(true);

				end_imgui();
				skip_frame = true;
				return;
			}
			ImGui::Text("Adapter output description");
			ImGui::Text(("Resolution: " + std::to_string(mode_desc.Width) + ", " + std::to_string(mode_desc.Height)).c_str());
			ImGui::Checkbox("Restrict present to adapter output", &restrict_present_to_adapter_output);
		}

		{ // Viewport & scissor rect
			static int vec[2]{(int)viewport.Width, (int)viewport.Height};
			ImGui::InputInt2("Viewport & scissor rect size", vec);
			if (ImGui::Button("Set viewport & scissor rect size")) {
				set_viewport_and_scissor_rect(UVector2{(UINT)vec[0], (UINT)vec[1]});
			}
		}

		ImGui::TreePop();
	}
}

void Renderer::compile() {
	HPEW(command_allocator->Reset());
	HPEW(command_list->Reset(command_allocator.Get(), nullptr));
	
	scene.compile();

	for (auto &mesh : scene.static_meshes) {
		//mesh->component->compile();
		mesh->material.component->pipeline.compile(device, command_list, msaa_sample_desc, blend_desc, descriptor_heap);
	}

	if (scene.sky.component != nullptr) {
		//scene.sky.component->compile();
		scene.sky.component->pipeline.compile(device, command_list, msaa_sample_desc, blend_desc, descriptor_heap);
	}
	
	scene.set_resources();
	scene.update(resolution);

	HPEW(command_list->Close());
	execute_command_list();
	signal();
}

void Renderer::render(float dt) {
	setup_imgui_section();

	if (skip_frame) return;

	scene.compile();
	scene.update(resolution);

	wait_for_previous_frame();
	
	resource_manager->release_all_resources();

	HPEW(command_allocator->Reset());
	HPEW(command_list->Reset(command_allocator.Get(), nullptr));

	bool msaa{msaa_sample_desc.Count > sample_desc.Count};

	CD3DX12_RESOURCE_BARRIER barrier{};
	if (msaa) {
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(msaa_render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_RESOLVE_SOURCE, D3D12_RESOURCE_STATE_RENDER_TARGET);
		command_list->ResourceBarrier(1, &barrier);

		barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		command_list->ResourceBarrier(1, &barrier);
	} else {
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
		command_list->ResourceBarrier(1, &barrier);
	}

	command_list->RSSetViewports(1u, &viewport);
	command_list->RSSetScissorRects(1u, &scissor_rect);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), (msaa ? frame_index+NUMBER_OF_BUFFERS : frame_index), rtv_descriptor_size);
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsv_handle(depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

	command_list->OMSetRenderTargets(1u, &rtv_handle, false, &dsv_handle);

	if (clear_render_target) {
		float color[4]{background_color.r, background_color.g, background_color.b, background_color.a};
		command_list->ClearRenderTargetView(rtv_handle, color, 0, nullptr);
	}
	command_list->ClearDepthStencilView(depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
	
	ID3D12DescriptorHeap* dh[] = { descriptor_heap.get().Get() };
	command_list->SetDescriptorHeaps(_countof(dh), dh);
	
	// OpenGL shadow mapping video
	// https://www.youtube.com/watch?v=kCCsko29pv0

	if (scene.sky.component != nullptr)
		scene.sky.component->pipeline.run(device, command_list, descriptor_heap);
	
	for (auto &mesh : scene.static_meshes) {
		mesh->material.component->pipeline.run(device, command_list, descriptor_heap);
	}

	// Render Dear ImGui graphics
	ImGui::End();
	ImGui::Render();
	ID3D12DescriptorHeap* imguidh[] = { descriptor_heap.get().Get() };
	command_list->SetDescriptorHeaps(_countof(imguidh), imguidh);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list.Get());

	if (msaa) {
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(msaa_render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_SOURCE);
		command_list->ResourceBarrier(1u, &barrier);

		barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_RESOLVE_DEST);
		command_list->ResourceBarrier(1u, &barrier);

		command_list->ResolveSubresource(render_targets[frame_index].Get(), 0u, msaa_render_targets[frame_index].Get(), 0u, DXGI_FORMAT_R8G8B8A8_UNORM);
	
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_RESOLVE_DEST, D3D12_RESOURCE_STATE_PRESENT);
		command_list->ResourceBarrier(1u, &barrier);
	} else {
		barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
		command_list->ResourceBarrier(1u, &barrier);
	}

	HPEW(command_list->Close());
}

void Renderer::present() {
	if (skip_frame) { // Whenever we need to skip a frame its because of stupid imgui
		skip_frame = false;
		return;
	}

	execute_command_list();

	signal();
	
	HPEW(swap_chain->Present(
		vsync,
		(restrict_present_to_adapter_output && !fullscreen ? DXGI_PRESENT_RESTRICT_TO_OUTPUT : 0u)
	));
}

//#include <conio.h>

void Renderer::tick(float dt) {
	render(dt);
	present();
}

void Renderer::clean_up(bool clean_scene) {
	flush_gpu();

	if (clean_scene) scene.clean_up();

	CloseHandle(fence_event);

	if (is_fullscreen())
		set_fullscreen(false);

	resource_manager->release_all_resources();

	descriptor_heap.clean_up();

	adapter.clean_up();
	adapter_output.clean_up();

	factory.Reset();

	debug_device.Reset();
	debug_command_list.Reset();
	debug_command_queue.Reset();

	debug_interface.Reset();
	info_queue.Reset();

	depth_stencil_buffer.Reset();
	depth_stencil_descriptor_heap.Reset();

	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		render_targets[i].Reset();
		command_allocator.Reset();
		fence.Reset();
	}

	device.Reset();
	swap_chain.Reset();
	command_queue.Reset();
	rtv_descriptor_heap.Reset();
	command_list.Reset();

	HPEW(dxgi_debug->ReportLiveObjects(DXGI_DEBUG_DXGI, DXGI_DEBUG_RLO_ALL));
	dxgi_debug.Reset();
}

void Renderer::wait_for_previous_frame() {
	set_frame_index();

	// if the current fences value is still less than "fencesValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fences, fencesValue)" command
	if (fence->GetCompletedValue() < fence_value) {
		// we have the fences create an event which is signaled once the fences's current value is "fence_values"
		HPEW(fence->SetEventOnCompletion(fence_value, fence_event));

		// We will wait until the fences has triggered the event that it's current value has reached "fence_values". once it's value
		// has reached "fencesValue", we know the command queue has finished executing
		WaitForSingleObject(fence_event, INFINITE);
	}

	// increment fence_values for next frame
	fence_value++;
}

void Renderer::wait_for_fence_gpu() {
	HPEW(command_queue->Wait(fence.Get(), fence_value));
}

void Renderer::flush_gpu() {
	uint64_t fenceValueForSignal = ++fence_value;
	HPEW(command_queue->Signal(fence.Get(), fenceValueForSignal));
	if (fence->GetCompletedValue() < fence_value) {
		HPEW(fence->SetEventOnCompletion(fenceValueForSignal, fence_event));
		WaitForSingleObject(fence_event, INFINITE);
	}
	frame_index = 0;
}

void Renderer::execute_command_list() {
	// create an array of command lists (only one command list here)
	ID3D12CommandList* const command_lists[] = { command_list.Get() };

	// execute the array of command lists
	command_queue->ExecuteCommandLists((UINT)std::size(command_lists), command_lists);
}

void Renderer::signal() {
	// this command goes in at the end of our command queue. we will know when our command queue 
	// has finished because the fences value will be set to "fence_values" from the GPU since the command
	// queue is being executed on the GPU
	HPEW(command_queue->Signal(fence.Get(), fence_value));
}

void Renderer::set_frame_index() {
	// swap the current rtv buffer index so we draw on the correct buffer
	frame_index = swap_chain->GetCurrentBackBufferIndex();
}

void Renderer::resize(const UVector2 &size) {
	flush_gpu();
	for (UINT i = 0; i < NUMBER_OF_BUFFERS; i++) render_targets[i].Reset();
	for (UINT i = 0; i < NUMBER_OF_BUFFERS; i++) msaa_render_targets[i].Reset();
	depth_stencil_buffer.Reset();

	HPEW(swap_chain->ResizeBuffers(NUMBER_OF_BUFFERS, size.x, size.y, DXGI_FORMAT_R8G8B8A8_UNORM, NULL));
	
	create_rtvs(size);
	set_viewport_and_scissor_rect(size);
	create_depth_stencil(size);
}

void Renderer::set_msaa_sample_count(UINT count) {
	flush_gpu();
	for (UINT i = 0; i < NUMBER_OF_BUFFERS; i++) render_targets[i].Reset();
	for (UINT i = 0; i < NUMBER_OF_BUFFERS; i++) msaa_render_targets[i].Reset();
	depth_stencil_buffer.Reset();
	EditorGUI::clean_up();

	msaa_sample_desc.Count = count;

	auto size = get_client_size(window);
	create_rtvs(size);
	create_depth_stencil(size);
	scene.refresh_pipelines(device, msaa_sample_desc, blend_desc);
	EditorGUI::init_with_renderer(window, device.Get(), NUMBER_OF_BUFFERS, msaa_sample_desc, descriptor_heap.get().Get());
}

void Renderer::set_fullscreen(bool fullscreen) {
	this->fullscreen = fullscreen;
	HPEW(swap_chain->SetFullscreenState(fullscreen, (fullscreen ? adapter_output.adapter_output.Get() : NULL)));
}

void Renderer::toggle_fullscreen() {
	set_fullscreen(!fullscreen);
}

bool Renderer::is_fullscreen() const noexcept {
	return fullscreen;
}

UVector2 Renderer::get_resolution() const noexcept {
	return resolution;
}

void Renderer::set_resolution(const UVector2 &resolution) {
	this->resolution = resolution;
	if (fullscreen) resize(resolution);
}
