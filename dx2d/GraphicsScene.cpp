#include "GraphicsScene.h"

GraphicsScene::GraphicsScene(HWND window, const ObjectVector &objects)
	: window(window), objects(objects) {

	create_essentials();
}

void GraphicsScene::tick() {
	// The first step in rendering is that we reset both the command allocator
	// and command list memory. You will notice here we use a pipeline that is currently NULL.
	// This is because pipelines require shaders and extra setup
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(command_allocator->Reset());
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(command_list->Reset(command_allocator.Get(), pipeline_state.Get()));

	// The second step is to use a resource barrier to synchronize/transition the next
	// back buffer for rendering. We then set that as a step in the command list.

	// Record commands in the command list now.
	// Start by setting the resource barrier.
	D3D12_RESOURCE_BARRIER barrier;
	barrier.Flags = D3D12_RESOURCE_BARRIER_FLAG_NONE;
	barrier.Transition.pResource = back_buffer_render_target[buffer_index].Get();
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_PRESENT;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.Subresource = D3D12_RESOURCE_BARRIER_ALL_SUBRESOURCES;
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	command_list->ResourceBarrier(1, &barrier);

	// The third step is to get the back buffer view handle and then set the back buffer
	// as the render target.

	// Get the render target view handle for the current back buffer.
	D3D12_CPU_DESCRIPTOR_HANDLE render_target_view_handle;
	UINT render_target_view_descriptor_size;
	render_target_view_handle = render_target_view_heap->GetCPUDescriptorHandleForHeapStart();
	render_target_view_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	if (buffer_index == 1) {
		render_target_view_handle.ptr += render_target_view_descriptor_size;
	}

	// Set the back buffer as the render target.
	command_list->OMSetRenderTargets(1, &render_target_view_handle, FALSE, NULL);

	// In the fourth step we set the clear color to grey and clear the render target
	// using that color and submit that to the command list.

	// Then set the color to clear the window to.
	float color[4] = { background_color.r, background_color.g, background_color.b, 1.0f };
	command_list->ClearRenderTargetView(render_target_view_handle, color, 0, NULL);

	// And finally we then set the state of the back buffer to transition into a
	// presenting state and store that in the command list.

	// Indicate that the back buffer will now be used to present.
	barrier.Transition.StateBefore = D3D12_RESOURCE_STATE_RENDER_TARGET;
	barrier.Transition.StateAfter = D3D12_RESOURCE_STATE_PRESENT;
	command_list->ResourceBarrier(1, &barrier);

	// Once we are done our rendering list we close the command list and then submit
	// it to the command queue to execute that list for us.

	// Close the list of commands.
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(command_list->Close());

	// Load the command list array (only one command list for now).
	ID3D12CommandList* command_lists[1];
	command_lists[0] = command_list.Get();

	// Execute the list of commands.
	command_queue->ExecuteCommandLists(1, command_lists);

	// We then call the swap chain to present the rendered frame to the screen.

	// Finally present the back buffer to the screen since rendering is complete.

	if (vsync_enabled) {
		// Lock to screen refresh rate.
		HANDLE_POSSIBLE_EXCEPTION_WINDOWS(swap_chain->Present(1, 0));
	} else {
		// Present as fast as possible.
		HANDLE_POSSIBLE_EXCEPTION_WINDOWS(swap_chain->Present(0, 0));
	}

	// Then we setup the fence to synchronize and let us know when the GPU is complete
	// rendering. For now we just wait infinitely until it's done this single
	// command list. However you can get optimiziations by doing other processing while
	// waiting for the GPU to finish.

	// Signal and increment the fence value.
	ULONGLONG fence_to_wait_for;
	fence_to_wait_for = fence_value;
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(command_queue->Signal(fence.Get(), fence_to_wait_for));
	fence_value++;

	// Wait until the GPU is done rendering.
	if (fence->GetCompletedValue() < fence_to_wait_for) {
		fence->SetEventOnCompletion(fence_to_wait_for, fence_event);
		WaitForSingleObject(fence_event, INFINITE);
	}

	// For the next frame swap to the other back buffer using the alternating index.

	// Alternate the back buffer index back and forth between 0 and 1 each frame.
	buffer_index = !buffer_index;
}

//D3D11_PRIMITIVE_TOPOLOGY GraphicsScene::get_primitive_topology() const noexcept {
//	return primitive_topology;
//}
//
//void GraphicsScene::set_primitive_topology(D3D11_PRIMITIVE_TOPOLOGY primitive_topology) noexcept {
//	this->primitive_topology = primitive_topology;
//	context->IASetPrimitiveTopology(primitive_topology);
//}
//
//Color GraphicsScene::get_background_color() const noexcept {
//	return background_color * 255;
//}
//
//void GraphicsScene::set_background_color(Color background_color) noexcept {
//	this->background_color = color_to_fcolor(background_color) / 255;
//}
//
//D3D11_VIEWPORT GraphicsScene::get_viewport() const noexcept {
//	return viewport;
//}
//
//void GraphicsScene::set_viewport(D3D11_VIEWPORT viewport) noexcept {
//	this->viewport = viewport;
//	context->RSSetViewports(1u, &viewport);
//}

void GraphicsScene::create_essentials() {
	HANDLE_POSSIBLE_EXCEPTION(D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
		__uuidof(ID3D12Device),
		(void**)&device
	));

	D3D12_COMMAND_QUEUE_DESC command_queue_description = { };
	command_queue_description.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
	command_queue_description.Priority = D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	command_queue_description.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
	command_queue_description.NodeMask = 0;

	// Create the command queue.
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(device->CreateCommandQueue(&command_queue_description, __uuidof(ID3D12CommandQueue), (void**)&command_queue));

	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory));
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(factory->EnumAdapters(0, &adapter));
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(adapter->EnumOutputs(0, &adapter_output));

	DXGI_SWAP_CHAIN_DESC swap_chain_description = { };
	swap_chain_description.BufferDesc.Width = resolution.x;
	swap_chain_description.BufferDesc.Height = resolution.y;
	swap_chain_description.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM; // Or DXGI_FORMAT_B8G8R8A8_UNORM? // Layout of pixels
	swap_chain_description.BufferDesc.RefreshRate.Numerator = 0; // Pick whatever is already there
	swap_chain_description.BufferDesc.RefreshRate.Denominator = 0; // Pick whatever is already there
	swap_chain_description.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED; // Don't need scaling
	swap_chain_description.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Scanline order (Interlaced, etc)
	swap_chain_description.SampleDesc.Count = 1; // No MSAA
	swap_chain_description.SampleDesc.Quality = 0; // No MSAA
	swap_chain_description.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT; // Use buffer as render target
	swap_chain_description.BufferCount = NUMBER_OF_BUFFERS; // Two buffers
	swap_chain_description.OutputWindow = window; // Use this window
	swap_chain_description.Windowed = true; // Is it windowed?
	swap_chain_description.SwapEffect = DXGI_SWAP_EFFECT::DXGI_SWAP_EFFECT_FLIP_DISCARD; // Swap effect that usually has the best performance
	swap_chain_description.Flags = 0u; // No flags

	IDXGISwapChain* temp_swap_chain = nullptr;
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(factory->CreateSwapChain(
		command_queue.Get(),
		&swap_chain_description,
		&temp_swap_chain
	));
	// Convert temporary swap chain to member swap chain
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(temp_swap_chain->QueryInterface(__uuidof(IDXGISwapChain3), (void**)&swap_chain));

	// Null out temporary swap chain
	temp_swap_chain = 0;

	// Set the number of descriptors to two for our two back buffers. Also set the heap type to render target views.
	D3D12_DESCRIPTOR_HEAP_DESC render_target_view_heap_description = { };
	render_target_view_heap_description.NumDescriptors = NUMBER_OF_BUFFERS;
	render_target_view_heap_description.Type = D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	render_target_view_heap_description.Flags = D3D12_DESCRIPTOR_HEAP_FLAGS::D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// Create the render target view heap for the back buffers.
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(device->CreateDescriptorHeap(&render_target_view_heap_description, __uuidof(ID3D12DescriptorHeap), (void**)&render_target_view_heap));

	// Get a handle to the starting memory location in the render target view heap to identify where the render target views will be located for the two back buffers.
	D3D12_CPU_DESCRIPTOR_HANDLE render_target_view_handle = render_target_view_heap->GetCPUDescriptorHandleForHeapStart();
	// Get the size of the memory location for the render target view descriptors.
	UINT render_target_view_descriptor_size = device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE::D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		// Get a pointer to the first back buffer from the swap chain.
		HANDLE_POSSIBLE_EXCEPTION_WINDOWS(swap_chain->GetBuffer(i, __uuidof(ID3D12Resource), (void**)&back_buffer_render_target[i]));
		// Create a render target view for the first back buffer.
		device->CreateRenderTargetView(back_buffer_render_target[i].Get(), NULL, render_target_view_handle);
		
		if (i < NUMBER_OF_BUFFERS-1) {
			// Increment the view handle to the next descriptor location in the render target view heap.
			render_target_view_handle.ptr += render_target_view_descriptor_size;
		}
	}

	// Finally get the initial index to which buffer is the current back buffer.
	buffer_index = swap_chain->GetCurrentBackBufferIndex();

	// Create a command allocator.
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&command_allocator));

	// Create a basic command list.
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator.Get(), NULL, __uuidof(ID3D12GraphicsCommandList), (void**)&command_list));

	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(command_list->Close());

	// Create a fence for GPU synchronization.
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&fence));
	// Create an event object for the fence.
	fence_event = CreateEventExW(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);

	/*CD3DX12_ROOT_SIGNATURE_DESC rootSignatureDesc;
	rootSignatureDesc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	ComPtr<ID3DBlob> error;
	D3D12SerializeRootSignature(&rootSignatureDesc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, &error);
	device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&root_signature));*/
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
