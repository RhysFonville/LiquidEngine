#include "Renderer.h"

Renderer::Renderer(HWND window) : window(window) {

	HPEW(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface)));
	debug_interface->EnableDebugLayer();
	//debug_interface->SetEnableGPUBasedValidation(true);

	DXGIGetDebugInterface1(NULL, IID_PPV_ARGS(&dxgi_debug));
	dxgi_debug->EnableLeakTrackingForThread();

	HPEW(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)));

	create_adapter_and_device();
	create_command_queue();
	create_swap_chain();
	create_descriptor_heap();
	create_rtvs();
	create_command_allocators();
	create_command_list();
	create_fences_and_fence_event();
	create_depth_stencil();
	create_descriptor_heaps();
	set_blend_state();
}

void Renderer::create_adapter_and_device() {
	int adapter_index = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0
	bool adapter_found = false; // set this to true when a good one was found

	// find first hardware gpu that supports d3d 12
	//while (factory->EnumAdapters1(adapter_index, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
	//	DXGI_ADAPTER_DESC1 desc;
	//	HPEW(adapter->GetDesc1(&desc));

	//	if (wcscmp(desc.Description, L"Software Adapter") == 0) {
	//		// we dont want a software device
	//		adapter_index++;
	//		continue;
	//	}

	//	// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
	//	HPEW(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr));
	//	if (SUCCEEDED(hpewr)) {
	//		adapter_found = true;
	//	}
	//	adapter_index++;
	//}

	//if (!adapter_found) {
	//	throw std::exception("Adapter (GPU) not found.");
	//}

	factory->EnumAdapters1(0u, adapter.GetAddressOf());
	adapter->EnumOutputs(0u, adapter_output.GetAddressOf());

	HPEW(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_12_0, IID_PPV_ARGS(&device)));
	device->SetName(L"Main Device");

	HPEW(device->QueryInterface(IID_PPV_ARGS(&debug_device)));
	HPEW(device->QueryInterface(IID_PPV_ARGS(&info_queue)));
	HPEW(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true));
	HPEW(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true));
	HPEW(info_queue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true));
}

void Renderer::create_command_queue() {
	const D3D12_COMMAND_QUEUE_DESC desc = {
		.Type = D3D12_COMMAND_LIST_TYPE_DIRECT,
		.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL,
		.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE,
		.NodeMask = 0,
	};

	HPEW(device->CreateCommandQueue(&desc, IID_PPV_ARGS(&command_queue))); // create the command queue
	HPEW(command_queue->SetName(L"Main Command Queue"));

	HPEW(command_queue->QueryInterface(IID_PPV_ARGS(&debug_command_queue)));
}

void Renderer::create_swap_chain() {
	// describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
	sample_desc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

	const DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {
		.Width = 1920u,
		.Height = 1080u,
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

	ComPtr<IDXGISwapChain1> swap_chain1;
	HPEW(factory->CreateSwapChainForHwnd(
		command_queue.Get(),
		window,
		&swapChainDesc,
		nullptr,
		nullptr,
		&swap_chain1
	));
	HPEW(swap_chain1.As(&swap_chain));

	frame_index = swap_chain->GetCurrentBackBufferIndex();

	DXGI_RGBA color = background_color;
	HPEW(swap_chain->SetBackgroundColor(&color));
}

void Renderer::create_descriptor_heap() {
	// describe an rtv descriptor heap and create
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
	rtv_heap_desc.NumDescriptors = NUMBER_OF_BUFFERS; // number of descriptors for this heap.
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // this heap is a render target view heap
														 // this heap will not be directly referenced by the shaders (not shader visible), as this will store the output from the pipeline
														 // otherwise we would set the heap's flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HPEW(device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtv_descriptor_heap)));

	// get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
	// descriptor sizes may vary from device to device, which is why there is no set size and we must ask the 
	// device to give us the size. we will use this size to increment a descriptor handle offset
	rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
}
	
void Renderer::create_rtvs() {
	// get a handle to the first descriptor in the descriptor heap.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each buffer
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		// first we get the n'th buffer in the swap chain and store it in the n'th
		// position of our ID3D12Resource array
		HPEW(swap_chain->GetBuffer(i, IID_PPV_ARGS(&render_targets[i])));

		// the we "create" a render target view which binds the swap chain buffer (ID3D12Resource[n]) to the rtv handle
		device->CreateRenderTargetView(render_targets[i].Get(), nullptr, rtv_handle);
		HPEW(render_targets[i]->SetName(string_to_wstring("Render Target #" + std::to_string(i)).c_str()));

		// we increment the rtv handle by the rtv descriptor size we got above
		rtv_handle.Offset(1, rtv_descriptor_size);
	}
}

void Renderer::create_command_allocators() {
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		HPEW(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocators[i])));
		HPEW(command_allocators[i]->SetName(string_to_wstring("Command Allocator #" + std::to_string(i)).c_str()));
	}
}

void Renderer::create_command_list() {
	// create the command list with the first allocator
	HPEW(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocators[0].Get(), NULL, IID_PPV_ARGS(&command_list)));
	HPEW(command_list->SetName(L"Main command list"));

	HPEW(command_list->QueryInterface(IID_PPV_ARGS(&debug_command_list)));

	command_list->Close();
}

void Renderer::create_fences_and_fence_event() {
	// create the fences
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		HPEW(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fences[i])));
		HPEW(fences[i]->SetName(string_to_wstring("Fence #" + std::to_string(i)).c_str()));

		fence_values[i] = 0; // set the initial fences value to 0
	}

	// create a handle to a fences event
	fence_event = CreateEventA(nullptr, false, false, nullptr);
	if (fence_event == nullptr) {
		throw std::exception("Failed to create fences event.");
	}
}

void Renderer::create_depth_stencil() {
	// create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer
	D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
	dsv_heap_desc.NumDescriptors = 1;
	dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HPEW(device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(&depth_stencil_descriptor_heap)));
	HPEW(depth_stencil_descriptor_heap->SetName(L"Depth Stencil Descriptor Heap"));

	D3D12_DEPTH_STENCIL_VIEW_DESC depth_stencil_desc = {};
	depth_stencil_desc.Format = DXGI_FORMAT_D32_FLOAT;
	depth_stencil_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_desc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depth_clear_value = {};
	depth_clear_value.Format = DXGI_FORMAT_D32_FLOAT;
	depth_clear_value.DepthStencil.Depth = 1.0f;
	depth_clear_value.DepthStencil.Stencil = 0;

	auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto tex = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, resolution.x, resolution.y, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
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

void Renderer::create_descriptor_heaps() {
	descriptor_heaps.compile(device);
}

void Renderer::set_blend_state() {
	blend_desc.AlphaToCoverageEnable = false;
	blend_desc.IndependentBlendEnable = false;
	blend_desc.RenderTarget[0] = D3D12_RENDER_TARGET_BLEND_DESC{
		.BlendEnable = true,
		.LogicOpEnable = false,
		.SrcBlend = D3D12_BLEND_SRC_ALPHA,
		.DestBlend = D3D12_BLEND_ONE,
		.BlendOp = D3D12_BLEND_OP_ADD,
		.SrcBlendAlpha = D3D12_BLEND_SRC_ALPHA,
		.DestBlendAlpha = D3D12_BLEND_ONE,
		.BlendOpAlpha = D3D12_BLEND_OP_ADD,
		.LogicOp = D3D12_LOGIC_OP_NOOP,
		.RenderTargetWriteMask = D3D12_COLOR_WRITE_ENABLE_ALL
	};
	blend_desc = CommonStates::AlphaBlend;
}

void Renderer::compile() {
	// reset command list and allocator   
	HPEW(command_allocators[frame_index]->Reset());
	HPEW(command_list->Reset(command_allocators[frame_index].Get(), nullptr));

	EditorGUI::init_with_renderer(window, device.Get(), NUMBER_OF_BUFFERS, descriptor_heaps[0].Get());
	descriptor_heaps.increment_heap_index();

	scene.compile();
	scene.update(resolution);

	int i = 0;
	for (RenderingStaticMesh &mesh : scene.static_meshes) {
		mesh.material.component->pipeline.root_signature.bind_root_constants<VSWVPConstants>(scene.camera.wvp_data, D3D12_SHADER_VISIBILITY_VERTEX, 16u);
		mesh.material.component->pipeline.root_signature.bind_root_constants<VSTransformConstants>(mesh.transform_data, D3D12_SHADER_VISIBILITY_VERTEX, 16u);
		mesh.material.component->pipeline.root_signature.bind_constant_buffer(mesh.lights_data, D3D12_SHADER_VISIBILITY_PIXEL);
		mesh.material.component->pipeline.root_signature.bind_root_constants(scene.camera.pos_data, D3D12_SHADER_VISIBILITY_PIXEL, 4u);
		mesh.material.component->pipeline.root_signature.bind_constant_buffer(mesh.material.material_data, D3D12_SHADER_VISIBILITY_PIXEL);

		mesh.component->compile();
		mesh.material.component->pipeline.compile(device, command_list, sample_desc, depth_stencil_desc, blend_desc, resolution, descriptor_heaps);

		*debug_console << "Compiling mesh #" << std::to_string(i) << '\n';
		i++;
	}

	if (scene.sky.component != nullptr) {
		scene.sky.component->pipeline.root_signature.bind_root_constants<VSWVPConstants>(scene.sky.wvp_data, D3D12_SHADER_VISIBILITY_VERTEX, 16u);
		scene.sky.component->pipeline.root_signature.bind_root_constants<VSTransformConstants>(scene.sky.transform_data, D3D12_SHADER_VISIBILITY_VERTEX, 16u);
		scene.sky.component->pipeline.root_signature.bind_constant_buffer<PSSkyCB>(scene.sky.data, D3D12_SHADER_VISIBILITY_PIXEL);
		scene.sky.component->compile();
		scene.sky.component->pipeline.compile(device, command_list, sample_desc, D3D12_DEPTH_STENCIL_DESC{}, blend_desc, resolution, descriptor_heaps);
	}

	HPEW(command_list->Close());
	execute_command_list();

	increment_fence();

	ResourceManager::Release::release_all_resources();
}

void Renderer::update(float dt) {
	// We have to wait for the gpu to finish with the command allocator before we reset it
	wait_for_previous_frame();

	// we can only reset an allocator once the gpu is done with it
	// resetting an allocator frees the memory that the command list was stored in
	HPEW(command_allocators[frame_index]->Reset());

	// reset the command list. by resetting the command list we are putting it into
	// a recording state so we can start recording commands into the command allocator.
	// the command allocator that we reference here may have multiple command lists
	// associated with it, but only one can be recording at any time. Make sure
	// that any other command lists associated to this command allocator are in
	// the closed state (not recording).
	// Here you will pass an initial pipeline state object as the second parameter,

	HPEW(command_list->Reset(command_allocators[frame_index].Get(), nullptr));

	// here we start recording commands into the commandList (which all the commands will be stored in the commandAllocator)

	// transition the "frame_index" render target from the present state to the render target state so the command list draws to it starting from here
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	command_list->ResourceBarrier(1, &barrier);

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), frame_index, rtv_descriptor_size);

	// get a handle to the depth/stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsv_handle(depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	command_list->OMSetRenderTargets(1, &rtv_handle, false, &dsv_handle);

	// Clear the render target by using the ClearRenderTargetView command
	float color[4]{background_color.r, background_color.g, background_color.b, background_color.a};
	if (ImGui::InputFloat4("Background color", color)) {
		background_color = FColor{color[0], color[1], color[2], color[3]};
	}
	command_list->ClearRenderTargetView(rtv_handle, color, 0, nullptr);

	command_list->ClearDepthStencilView(depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// set constant buffer descriptor heap
	ID3D12DescriptorHeap* dh[] = { descriptor_heaps[frame_index].Get() };
	command_list->SetDescriptorHeaps(_countof(dh), dh);

	scene.update(resolution);
	
	if (scene.sky.component != nullptr)
		scene.sky.component->pipeline.run(device, command_list, frame_index, descriptor_heaps);
	
	for (RenderingStaticMesh &mesh : scene.static_meshes) {
		mesh.material.component->pipeline.run(device, command_list, frame_index, descriptor_heaps);
	}

	// Render Dear ImGui graphics
	ImGui::End();
	ImGui::Render();
	ID3D12DescriptorHeap* imguidh[] = { descriptor_heaps[0].Get() };
	command_list->SetDescriptorHeaps(_countof(imguidh), imguidh);
	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), command_list.Get());

	// transition the "frame_index" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
	// warning if present is called on the render target when it's not in the present state
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	command_list->ResourceBarrier(1, &barrier);

	HPEW(command_list->Close());

	ResourceManager::Release::release_all_resources();
}

void Renderer::render() {
	execute_command_list();

	// this command goes in at the end of our command queue. we will know when our command queue 
	// has finished because the fences value will be set to "fence_values" from the GPU since the command
	// queue is being executed on the GPU
	HPEW(command_queue->Signal(fences[frame_index].Get(), fence_values[frame_index]));

	// present the current backbuffer
	HPEW(swap_chain->Present(0, 0));
}

void Renderer::tick(float dt) {
	update(dt);
	render();
}

void Renderer::clean_up() {
	wait_for_previous_frame();

	CloseHandle(fence_event);

	flush_gpu();

	// get swapchain out of full screen before exiting
	if (is_fullscreen())
		set_fullscreen(false);

	scene.clean_up();

	ResourceManager::Release::release_all_resources();

	adapter_output.Reset();
	adapter.Reset();

	factory.Reset();

	debug_device.Reset();
	debug_command_list.Reset();
	debug_command_list.Reset();
	debug_command_queue.Reset();
	debug_interface.Reset();

	device.Reset();
	swap_chain.Reset();
	command_queue.Reset();
	rtv_descriptor_heap.Reset();
	command_list.Reset();

	depth_stencil_buffer.Reset();
	depth_stencil_descriptor_heap.Reset();

	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		render_targets[i].Reset();
		command_allocators[i].Reset();
		fences[i].Reset();
	}

	HPEW(dxgi_debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY));
	debug_interface.Reset();
}

void Renderer::increment_fence() {
	fence_values[frame_index]++;
	HPEW(command_queue->Signal(fences[frame_index].Get(), fence_values[frame_index]));
}

void Renderer::wait_for_fence_cpu() {
	// if the current fences value is still less than "fencesValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fences, fencesValue)" command
	if (fences[frame_index]->GetCompletedValue() < fence_values[frame_index]) {
		// we have the fences create an event which is signaled once the fences's current value is "fence_values"
		HPEW(fences[frame_index]->SetEventOnCompletion(fence_values[frame_index], fence_event));

		// We will wait until the fences has triggered the event that it's current value has reached "fence_values". once it's value
		// has reached "fencesValue", we know the command queue has finished executing
		WaitForSingleObject(fence_event, INFINITE);
	}
}

void Renderer::wait_for_fence_gpu() {
	HPEW(command_queue->Wait(fences[frame_index].Get(), fence_values[frame_index]));
}

void Renderer::flush_gpu() {
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		uint64_t fenceValueForSignal = ++fence_values[i];
		HPEW(command_queue->Signal(fences[i].Get(), fenceValueForSignal));
		if (fences[i]->GetCompletedValue() < fence_values[i])
		{
			HPEW(fences[i]->SetEventOnCompletion(fenceValueForSignal, fence_event));
			WaitForSingleObject(fence_event, INFINITE);
		}
	}
	frame_index = 0;
}

void Renderer::wait_for_previous_frame() {
	// swap the current rtv buffer index so we draw on the correct buffer
	frame_index = swap_chain->GetCurrentBackBufferIndex();

	wait_for_fence_cpu();

	// increment fence_values for next frame
	fence_values[frame_index]++;
}

void Renderer::execute_command_list() {
	// create an array of command lists (only one command list here)
	ID3D12CommandList* const commandLists[] = { command_list.Get() };

	// execute the array of command lists
	command_queue->ExecuteCommandLists((UINT)std::size(commandLists), commandLists);
}

void Renderer::resize(const UVector2 &size) {
	flush_gpu();
	for (UINT i = 0; i < NUMBER_OF_BUFFERS; i++) render_targets[i].Reset();
	HPEW(swap_chain->ResizeBuffers(NUMBER_OF_BUFFERS, size.x, size.y, DXGI_FORMAT_R8G8B8A8_UNORM, NULL));
	create_rtvs();
}

void Renderer::set_fullscreen(bool fullscreen) {
	this->fullscreen = fullscreen;
	HPEW(swap_chain->SetFullscreenState(fullscreen, NULL));
}

void Renderer::toggle_fullscreen() {
	fullscreen = !fullscreen;
	HPEW(swap_chain->SetFullscreenState(fullscreen, NULL));
}

bool Renderer::is_fullscreen() const noexcept {
	return fullscreen;
}

UVector2 Renderer::get_resolution() const noexcept {
	return resolution;
}
