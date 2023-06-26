#include "GraphicsScene.h"

GraphicsScene::GraphicsScene(HWND window, const std::vector<AppearanceComponent*> &appearances)
	: window(window), appearances(appearances) {

	HPEW(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface)));
	debug_interface->EnableDebugLayer();

	create_adapter_and_device();
	create_command_queue();
	create_swap_chain();
	create_back_buffers_and_rtv_with_descriptor_heap();
	create_command_allocators();
	create_command_list();
	create_fences_and_fences_event();
}

void GraphicsScene::create_adapter_and_device() {
	HPEW(CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(&factory)));

	int adapter_index = 0; // we'll start looking for directx 12  compatible graphics devices starting at index 0
	bool adapter_found = false; // set this to true when a good one was found

	// find first hardware gpu that supports d3d 12
	while (factory->EnumAdapters1(adapter_index, adapter.GetAddressOf()) != DXGI_ERROR_NOT_FOUND) {
		DXGI_ADAPTER_DESC1 desc;
		HPEW(adapter->GetDesc1(&desc));

		if (wcscmp(desc.Description, L"Software Adapter") == 0) {
			// we dont want a software device
			adapter_index++;
			continue;
		}

		// we want a device that is compatible with direct3d 12 (feature level 11 or higher)
		HPEW(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, _uuidof(ID3D12Device), nullptr));
		if (SUCCEEDED(hpewr)) {
			adapter_found = true;
		}
		adapter_index++;
	}

	if (!adapter_found) {
		throw std::exception("Adapter (GPU) not found.");
	}

	HPEW(D3D12CreateDevice(adapter.Get(), D3D_FEATURE_LEVEL_11_0, IID_PPV_ARGS(&device)));

	device->QueryInterface(IID_PPV_ARGS(&debug_device));
}

void GraphicsScene::create_command_queue() {
	D3D12_COMMAND_QUEUE_DESC command_queue_desc = { }; // we will be using all the default values

	HPEW(device->CreateCommandQueue(&command_queue_desc, IID_PPV_ARGS(&command_queue))); // create the command queue

	command_queue->QueryInterface(IID_PPV_ARGS(&debug_command_queue));
}

void GraphicsScene::create_swap_chain() {
	back_buffer_desc.Width = resolution.x; // buffer width
	back_buffer_desc.Height = resolution.y; // buffer height
	back_buffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

	// describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
	sample_desc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount = GraphicsPipeline::NUMBER_OF_BUFFERS; // number of buffers we have
	swap_chain_desc.BufferDesc = back_buffer_desc; // our back buffer desc
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
	swap_chain_desc.OutputWindow = window; // handle to our window
	swap_chain_desc.SampleDesc = sample_desc; // our multi-sampling desc
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	swap_chain_desc.Windowed = !fullscreen;

	IDXGISwapChain* temp_swap_chain;

	factory->CreateSwapChain(
		command_queue.Get(), // the queue will be flushed once the swap chain is created
		&swap_chain_desc, // give it the swap chain desc we created above
		&temp_swap_chain // store the created swap chain in a temp IDXGISwapChain interface
	);

	swap_chain = static_cast<IDXGISwapChain3*>(temp_swap_chain);

	frame_index = swap_chain->GetCurrentBackBufferIndex();

	DXGI_RGBA color = background_color;
	swap_chain->SetBackgroundColor(&color);
}

void GraphicsScene::create_back_buffers_and_rtv_with_descriptor_heap() {
	// describe an rtv descriptor heap and create
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
	rtv_heap_desc.NumDescriptors = GraphicsPipeline::NUMBER_OF_BUFFERS; // number of descriptors for this heap.
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // this heap is a render target view heap

														 // This heap will not be directly referenced by the shaders (not shader visible), as this will store the output from the pipeline
														 // otherwise we would set the heap's flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HPEW(device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtv_descriptor_heap)));

	// get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
	// descriptor sizes may vary from device to device, which is why there is no set size and we must ask the 
	// device to give us the size. we will use this size to increment a descriptor handle offset
	rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// get a handle to the first descriptor in the descriptor heap.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each buffer
	for (int i = 0; i < GraphicsPipeline::NUMBER_OF_BUFFERS; i++) {
		// first we get the n'th buffer in the swap chain and store it in the n'th
		// position of our ID3D12Resource array
		HPEW(swap_chain->GetBuffer(i, IID_PPV_ARGS(&render_targets[i])));

		// the we "create" a render target view which binds the swap chain buffer (ID3D12Resource[n]) to the rtv handle
		device->CreateRenderTargetView(render_targets[i].Get(), nullptr, rtv_handle);

		// we increment the rtv handle by the rtv descriptor size we got above
		rtv_handle.Offset(1, rtv_descriptor_size);
	}
}

void GraphicsScene::create_command_allocators() {
	for (int i = 0; i < GraphicsPipeline::NUMBER_OF_BUFFERS; i++) {
		HPEW(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocators[i])));
	}
}

void GraphicsScene::create_command_list() {
	// create the command list with the first allocator
	HPEW(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocators[0].Get(), NULL, IID_PPV_ARGS(&command_list)));

	command_list->QueryInterface(IID_PPV_ARGS(&debug_command_list));
}

void GraphicsScene::create_fences_and_fences_event() {
	// create the fences
	for (int i = 0; i < GraphicsPipeline::NUMBER_OF_BUFFERS; i++) {
		HPEW(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fences[i])));
		fence_values[i] = 0; // set the initial fences value to 0
	}

	// create a handle to a fences event
	fences_event = CreateEventA(nullptr, false, false, nullptr);
	if (fences_event == nullptr) {
		throw std::exception("Failed to create fences event.");
	}
}

void GraphicsScene::compile() {
	for (AppearanceComponent *appearance : appearances) {
		appearance->pipeline.root_signature.bind_root_constants<PerFrameVSCB>(cbs.per_frame_vs, D3D12_SHADER_VISIBILITY_VERTEX, 16u);
		appearance->pipeline.root_signature.bind_root_constants<PerObjectVSCB>(cbs.per_object_vs, D3D12_SHADER_VISIBILITY_VERTEX, 16u);
		appearance->pipeline.root_signature.bind_constant_buffer(cbs.per_frame_ps.cb, D3D12_SHADER_VISIBILITY_PIXEL);
		appearance->pipeline.root_signature.bind_constant_buffer(cbs.per_object_ps.cb, D3D12_SHADER_VISIBILITY_PIXEL);

		appearance->compile(device, command_list, sample_desc, resolution);
	}

	HPEW(command_list->Close());

	ID3D12CommandList* command_lists[] = { command_list.Get() };
	command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

	// increment the fence value now, otherwise the buffer might not be uploaded by the time we start drawing
	increment_fence();
}

void GraphicsScene::update() {
	// We have to wait for the gpu to finish with the command allocator before we reset it
	wait_for_previous_frame();

	if (camera != nullptr) {
		camera->update(UVector2_to_FVector2(resolution));
		cbs.per_frame_vs.WVP = XMMatrixTranspose(camera->WVP);
		cbs.per_frame_ps.obj->camera_position = camera->get_position();
	}

	std::vector<DXDLData> dl(MAX_LIGHTS_PER_TYPE);
	std::vector<DXPLData> pl(MAX_LIGHTS_PER_TYPE);
	std::vector<DXSLData> sl(MAX_LIGHTS_PER_TYPE);
	UINT dl_count = 0;
	UINT pl_count = 0;
	UINT sl_count = 0;
	for (int i = 0; i < MAX_LIGHTS_PER_TYPE; i++) {
		dl[i].null = 1;
		pl[i].null = 1;
		sl[i].null = 1;
	}
	
	for (int i = 0; i < MAX_LIGHTS_PER_TYPE && i < lights.size(); i++) {
		if (lights[i]->get_type() == Component::Type::DirectionalLightComponent) {
			auto data = ((DirectionalLightComponent*)lights[i])->data;
			dl[dl_count] = DXDLData(data);
			dl_count++;
		}
		if (lights[i]->get_type() == Component::Type::PointLightComponent) {
			auto data = ((PointLightComponent*)lights[i])->data;
			pl[pl_count] = DXPLData(data, lights[i]->get_position());
			pl_count++;
		}
		if (lights[i]->get_type() == Component::Type::SpotlightComponent) {
			auto data = ((SpotlightComponent*)lights[i])->data;
			sl[sl_count] = DXSLData(data);
			sl_count++;
		}
	}

	std::copy(dl.begin(), dl.end(), cbs.per_frame_ps.obj->directional_lights);
	std::copy(pl.begin(), pl.end(), cbs.per_frame_ps.obj->point_lights);
	std::copy(sl.begin(), sl.end(), cbs.per_frame_ps.obj->spotlights);
	cbs.per_frame_ps.obj->directional_light_count = dl_count;
	cbs.per_frame_ps.obj->point_light_count = pl_count;
	cbs.per_frame_ps.obj->spotlight_count = sl_count;

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

	// Clear the render target by using the ClearRenderTargetView command
	const float color[4] = { background_color.r, background_color.g,
							 background_color.b, background_color.a };
	command_list->ClearRenderTargetView(rtv_handle, color, 0, nullptr);

	for (AppearanceComponent *appearance : appearances) {
		cbs.per_object_vs.transform = appearance->get_mesh()->get_transform();
		cbs.per_object_ps.obj->material = appearance->material.data;
		appearance->pipeline.run(device, command_list, rtv_handle, frame_index);
	}

	// transition the "frame_index" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
	// warning if present is called on the render target when it's not in the present state
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	command_list->ResourceBarrier(1, &barrier);

	HPEW(command_list->Close());
}

void GraphicsScene::render() {
	// create an array of command lists (only one command list here)
	ID3D12CommandList* command_lists[] = { command_list.Get() };

	// execute the array of command lists
	command_queue->ExecuteCommandLists(_countof(command_lists), command_lists);

	// this command goes in at the end of our command queue. we will know when our command queue 
	// has finished because the fences value will be set to "fence_values" from the GPU since the command
	// queue is being executed on the GPU
	HPEW(command_queue->Signal(fences[frame_index].Get(), fence_values[frame_index]));

	// present the current backbuffer
	HPEW(swap_chain->Present(0, 0));
}

void GraphicsScene::tick() {
	update();
	render();
}

void GraphicsScene::clean_up() {
	// wait for the gpu to finish all frames
	for (int i = 0; i < GraphicsPipeline::NUMBER_OF_BUFFERS; i++) {
		frame_index = i;
		wait_for_previous_frame();
	}

	// get swapchain out of full screen before exiting
	if (is_fullscreen())
		set_fullscreen(false);

	SAFE_RELEASE(device);
	SAFE_RELEASE(swap_chain);
	SAFE_RELEASE(command_queue);
	SAFE_RELEASE(rtv_descriptor_heap);
	SAFE_RELEASE(command_list);

	for (int i = 0; i < GraphicsPipeline::NUMBER_OF_BUFFERS; ++i) {
		SAFE_RELEASE(render_targets[i]);
		SAFE_RELEASE(command_allocators[i]);
		SAFE_RELEASE(fences[i]);
	}
}

void GraphicsScene::increment_fence() {
	fence_values[frame_index]++;
	HPEW(command_queue->Signal(fences[frame_index].Get(), fence_values[frame_index]));
}

void GraphicsScene::wait_for_fence_cpu() {
	// if the current fences value is still less than "fencesValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fences, fencesValue)" command
	if (fences[frame_index]->GetCompletedValue() < fence_values[frame_index]) {
		// we have the fences create an event which is signaled once the fences's current value is "fence_values"
		HPEW(fences[frame_index]->SetEventOnCompletion(fence_values[frame_index], fences_event));

		// We will wait until the fences has triggered the event that it's current value has reached "fence_values". once it's value
		// has reached "fencesValue", we know the command queue has finished executing
		WaitForSingleObject(fences_event, INFINITE);
	}
}

void GraphicsScene::wait_for_fence_gpu() {
	HPEW(command_queue->Wait(fences[frame_index].Get(), fence_values[frame_index]));
}

void GraphicsScene::wait_for_previous_frame() {
	// swap the current rtv buffer index so we draw on the correct buffer
	frame_index = swap_chain->GetCurrentBackBufferIndex();

	wait_for_fence_cpu();

	// increment fence_values for next frame
	fence_values[frame_index]++;
}

void GraphicsScene::set_fullscreen(bool fullscreen) {
	this->fullscreen = fullscreen;
	HPEW(swap_chain->SetFullscreenState(fullscreen, NULL));
}

void GraphicsScene::toggle_fullscreen() {
	fullscreen = !fullscreen;
	HPEW(swap_chain->SetFullscreenState(fullscreen, NULL));
}

bool GraphicsScene::is_fullscreen() const noexcept {
	return fullscreen;
}

UVector2 GraphicsScene::get_resolution() const noexcept {
	return resolution;
}

void GraphicsScene::set_resolution(const UVector2 &resolution, bool reset_om_viewing_settings) {
	this->resolution = resolution;
	
	back_buffer_desc.Width = resolution.x;
	back_buffer_desc.Height = resolution.y;

	HPEW(swap_chain->ResizeTarget(&back_buffer_desc));

	if (reset_om_viewing_settings) {
		// dfslkfgjhsdf
	}
}

//std::shared_ptr<CameraComponent> GraphicsScene::camera() const {
//	for (std::shared_ptr<Object> object : *objects) {
//		if (object->has_component(Component::Type::CameraComponent)) {
//			return object->get_component<CameraComponent>();
//		}
//	}
//	WARNING_MESSAGE("There is no camera in the scene.");
//	return nullptr;
//}
//
//std::vector<std::shared_ptr<DirectionalLightComponent>> GraphicsScene::directional_lights() const {
//	std::vector<std::shared_ptr<DirectionalLightComponent>> ret;
//	for (std::shared_ptr<Object> object : *objects) {
//		if (object->has_component(Component::Type::DirectionalLightComponent)) {
//			std::vector<std::shared_ptr<DirectionalLightComponent>> dls = object->get_components<DirectionalLightComponent>();
//			ret.insert(ret.end(), dls.begin(), dls.end());
//		}
//	}
//	return ret;
//}
//
//std::vector<std::shared_ptr<PointLightComponent>> GraphicsScene::point_lights() const {
//	std::vector<std::shared_ptr<PointLightComponent>> ret;
//	for (std::shared_ptr<Object> object : *objects) {
//		if (object->has_component(Component::Type::PointLightComponent)) {
//			std::vector<std::shared_ptr<PointLightComponent>> dls = object->get_components<PointLightComponent>();
//			ret.insert(ret.end(), dls.begin(), dls.end());
//		}
//	}
//	return ret;
//}
//
//std::vector<std::shared_ptr<SpotlightComponent>> GraphicsScene::spotlights() const {
//	std::vector<std::shared_ptr<SpotlightComponent>> ret;
//	for (std::shared_ptr<Object> object : *objects) {
//		if (object->has_component(Component::Type::SpotlightComponent)) {
//			std::vector<std::shared_ptr<SpotlightComponent>> dls = object->get_components<SpotlightComponent>();
//			ret.insert(ret.end(), dls.begin(), dls.end());
//		}
//	}
//	return ret;
//}
