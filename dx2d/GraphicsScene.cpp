#include "GraphicsScene.h"

GraphicsScene::GraphicsScene(HWND window, const ObjectVector &objects)
	: window(window), objects(objects) {
	initD3D();
}

void GraphicsScene::initD3D() {
	// -- Create the Device -- //

	HPEW(CreateDXGIFactory1(IID_PPV_ARGS(&factory)));

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

	// -- Create the Command Queue -- //

	D3D12_COMMAND_QUEUE_DESC cqDesc = { }; // we will be using all the default values

	HPEW(device->CreateCommandQueue(&cqDesc, IID_PPV_ARGS(&command_queue))); // create the command queue
	
	// -- Create the Swap Chain (triple buffering) -- //														 // -- Create the Swap Chain (double/tripple buffering) -- //

	DXGI_MODE_DESC back_buffer_desc = {}; // this is to describe our display mode
	back_buffer_desc.Width = resolution.x; // buffer width
	back_buffer_desc.Height = resolution.y; // buffer height
	back_buffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)
	
	// describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
	DXGI_SAMPLE_DESC sample_desc = {};
	sample_desc.Count = 1; // multisample count (no multisampling, so we just put 1, since we still need 1 sample)

	// Describe and create the swap chain.
	DXGI_SWAP_CHAIN_DESC swap_chain_desc = {};
	swap_chain_desc.BufferCount = NUMBER_OF_BUFFERS; // number of buffers we have
	swap_chain_desc.BufferDesc = back_buffer_desc; // our back buffer description
	swap_chain_desc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // this says the pipeline will render to this swap chain
	swap_chain_desc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD; // dxgi will discard the buffer (data) after we call present
	swap_chain_desc.OutputWindow = window; // handle to our window
	swap_chain_desc.SampleDesc = sample_desc; // our multi-sampling description
	swap_chain_desc.Windowed = !fullscreen; // set to true, then if in fullscreen must call SetFullScreenState with true for full screen to get uncapped fps

	IDXGISwapChain *temp_swap_chain;

	factory->CreateSwapChain(
		command_queue.Get(), // the queue will be flushed once the swap chain is created
		&swap_chain_desc, // give it the swap chain description we created above
		&temp_swap_chain // store the created swap chain in a temp IDXGISwapChain interface
	);

	swap_chain = static_cast<IDXGISwapChain3*>(temp_swap_chain);

	frame_index = swap_chain->GetCurrentBackBufferIndex();
	

	// -- Create the Back Buffers (render target views) Descriptor Heap -- //

	// describe an rtv descriptor heap and create
	D3D12_DESCRIPTOR_HEAP_DESC rtv_heap_desc = {};
	rtv_heap_desc.NumDescriptors = NUMBER_OF_BUFFERS; // number of descriptors for this heap.
	rtv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV; // this heap is a render target view heap

													   // This heap will not be directly referenced by the shaders (not shader visible), as this will store the output from the pipeline
													   // otherwise we would set the heap's flag to D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE
	rtv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HPEW(device->CreateDescriptorHeap(&rtv_heap_desc, IID_PPV_ARGS(&rtv_descriptor_heap)));

	// get the size of a descriptor in this heap (this is a rtv heap, so only rtv descriptors should be stored in it.
	// descriptor sizes may vary from device to device, which is why there is no set size and we must ask the 
	// device to give us the size. we will use this size to increment a descriptor handle offset
	rtv_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	// get a handle to the first descriptor in the descriptor heap. a handle is basically a pointer,
	// but we cannot literally use it like a c++ pointer.
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

	// Create a RTV for each buffer (double buffering is two buffers, tripple buffering is 3).
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		// first we get the n'th buffer in the swap chain and store it in the n'th
		// position of our ID3D12Resource array
		HPEW(swap_chain->GetBuffer(i, IID_PPV_ARGS(&render_targets[i])));

		// the we "create" a render target view which binds the swap chain buffer (ID3D12Resource[n]) to the rtv handle
		device->CreateRenderTargetView(render_targets[i].Get(), nullptr, rtv_handle);

		// we increment the rtv handle by the rtv descriptor size we got above
		rtv_handle.Offset(1, rtv_descriptor_size);
	}

	// -- Create the Command Allocators -- //

	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		HPEW(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator[i])));
	}

	// create the command list with the first allocator
	HPEW(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator[0].Get(), NULL, IID_PPV_ARGS(&command_list)));

	// command lists are created in the recording state. our main loop will set it up for recording again so close it now
	command_list->Close();

	// -- Create a Fence & Fence Event -- //

	// create the fences
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++)
	{
		HPEW(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence[i])));
		fence_value[i] = 0; // set the initial fence value to 0
	}

	// create a handle to a fence event
	fence_event = CreateEventA(nullptr, FALSE, FALSE, nullptr);
	if (fence_event == nullptr) {
		throw std::exception("Failed to create fence event.");
	}
}

void GraphicsScene::update_pipeline() {
	// We have to wait for the gpu to finish with the command allocator before we reset it
	wait_for_previous_frame();

	// we can only reset an allocator once the gpu is done with it
	// resetting an allocator frees the memory that the command list was stored in
	HPEW(command_allocator[frame_index]->Reset());

	// reset the command list. by resetting the command list we are putting it into
	// a recording state so we can start recording commands into the command allocator.
	// the command allocator that we reference here may have multiple command lists
	// associated with it, but only one can be recording at any time. Make sure
	// that any other command lists associated to this command allocator are in
	// the closed state (not recording).
	// Here you will pass an initial pipeline state object as the second parameter,
	// but in this tutorial we are only clearing the rtv, and do not actually need
	// anything but an initial default pipeline, which is what we get by setting
	// the second parameter to NULL
	HPEW(command_list->Reset(command_allocator[frame_index].Get(), NULL));

	// here we start recording commands into the commandList (which all the commands will be stored in the commandAllocator)

	// transition the "frameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	command_list->ResourceBarrier(1, &barrier);

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), frame_index, rtv_descriptor_size);

	// set the render target for the output merger stage (the output of the pipeline)
	command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, nullptr);

	// Clear the render target by using the ClearRenderTargetView command
	const float color[4] = { background_color.r, background_color.g,
							 background_color.b, background_color.a };
	command_list->ClearRenderTargetView(rtv_handle, color, 0, nullptr);

	// transition the "frameIndex" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
	// warning if present is called on the render target when it's not in the present state
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	command_list->ResourceBarrier(1, &barrier);

	HPEW(command_list->Close());
}

void GraphicsScene::render() {
	// create an array of command lists (only one command list here)
	ID3D12CommandList* ppCommandLists[] = { command_list.Get() };

	// execute the array of command lists
	command_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// this command goes in at the end of our command queue. we will know when our command queue 
	// has finished because the fence value will be set to "fenceValue" from the GPU since the command
	// queue is being executed on the GPU
	HPEW(command_queue->Signal(fence[frame_index].Get(), fence_value[frame_index]));

	// present the current backbuffer
	HPEW(swap_chain->Present(0, 0));
}

void GraphicsScene::tick() {
	update_pipeline();
	render();
}

void GraphicsScene::cleanup() {

}

void GraphicsScene::wait_for_previous_frame() {
	// swap the current rtv buffer index so we draw on the correct buffer
	frame_index = swap_chain->GetCurrentBackBufferIndex();

	// if the current fence value is still less than "fenceValue", then we know the GPU has not finished executing
	// the command queue since it has not reached the "commandQueue->Signal(fence, fenceValue)" command
	if (fence[frame_index]->GetCompletedValue() < fence_value[frame_index]) {
		// we have the fence create an event which is signaled once the fence's current value is "fenceValue"
		HPEW(fence[frame_index]->SetEventOnCompletion(fence_value[frame_index], fence_event));

		// We will wait until the fence has triggered the event that it's current value has reached "fenceValue". once it's value
		// has reached "fenceValue", we know the command queue has finished executing
		WaitForSingleObject(fence_event, INFINITE);
	}

	// increment fenceValue for next frame
	fence_value[frame_index]++;
}

std::shared_ptr<CameraComponent> GraphicsScene::camera() const {
	for (std::shared_ptr<Object> object : *objects) {
		if (object->has_component(Component::Type::CameraComponent)) {
			return object->get_component<CameraComponent>();
		}
	}
	WARNING_MESSAGE(L"There is no camera in the scene.");
	return nullptr;
}

std::vector<std::shared_ptr<DirectionalLightComponent>> GraphicsScene::directional_lights() const {
	std::vector<std::shared_ptr<DirectionalLightComponent>> ret;
	for (std::shared_ptr<Object> object : *objects) {
		if (object->has_component(Component::Type::DirectionalLightComponent)) {
			std::vector<std::shared_ptr<DirectionalLightComponent>> dls = object->get_components<DirectionalLightComponent>();
			ret.insert(ret.end(), dls.begin(), dls.end());
		}
	}
	return ret;
}

std::vector<std::shared_ptr<PointLightComponent>> GraphicsScene::point_lights() const {
	std::vector<std::shared_ptr<PointLightComponent>> ret;
	for (std::shared_ptr<Object> object : *objects) {
		if (object->has_component(Component::Type::PointLightComponent)) {
			std::vector<std::shared_ptr<PointLightComponent>> dls = object->get_components<PointLightComponent>();
			ret.insert(ret.end(), dls.begin(), dls.end());
		}
	}
	return ret;
}

std::vector<std::shared_ptr<SpotlightComponent>> GraphicsScene::spotlights() const {
	std::vector<std::shared_ptr<SpotlightComponent>> ret;
	for (std::shared_ptr<Object> object : *objects) {
		if (object->has_component(Component::Type::SpotlightComponent)) {
			std::vector<std::shared_ptr<SpotlightComponent>> dls = object->get_components<SpotlightComponent>();
			ret.insert(ret.end(), dls.begin(), dls.end());
		}
	}
	return ret;
}

std::shared_ptr<MeshComponent> GraphicsScene::obj_mesh(const Object &object) const {
	return object.get_component<MeshComponent>();
}
