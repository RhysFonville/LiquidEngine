#include "GraphicsScene.h"

GraphicsScene::GraphicsScene(HWND window, const ObjectVector &objects)
	: window(window), objects(objects) {
	create_essentials();
}

void GraphicsScene::compile() {
	
}

void GraphicsScene::tick() {
	UVector2 window_size = get_window_size(window);

	// The first step in rendering is that we reset both the command allocator
	// and command list memory. You will notice here we use a pipeline that is currently NULL.
	// This is because pipelines require shaders and extra setup
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(command_allocator->Reset());
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(command_list->Reset(command_allocator.Get(), pipeline_state.Get()));

	// The second step is to use a resource barrier to synchronize/transition the next
	// back buffer for rendering. We then set that as a step in the command list.

	// Record commands in the command list now.
	// Start by setting the resource barrier.
	// transition the "frameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(back_buffer_render_target[buffer_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
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


	// transition the vertex buffer data from copy destination state to vertex buffer state
	auto barriers = CD3DX12_RESOURCE_BARRIER::Transition(vertex_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	command_list->ResourceBarrier(1, &barriers);

	// draw triangle
	command_list->SetGraphicsRootSignature(root_signature.Get()); // set the root signature
	command_list->RSSetViewports(1, &viewport); // set the viewports
	command_list->RSSetScissorRects(1, &scissor_rect); // set the scissor rects
	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view); // set the vertex buffer (using the vertex buffer view)
	command_list->DrawInstanced(3, 1, 0, 0); // finally draw 3 vertices (draw the triangle)

	// And finally we then set the state of the back buffer to transition into a
	// presenting state and store that in the command list.

	// Indicate that the back buffer will now be used to present.
	barrier = CD3DX12_RESOURCE_BARRIER::Transition(back_buffer_render_target[buffer_index].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
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
#if defined(DEBUG) || defined(_DEBUG)
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface)));
	debug_interface->EnableDebugLayer();
#endif

	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(D3D12CreateDevice(
		nullptr,
		D3D_FEATURE_LEVEL::D3D_FEATURE_LEVEL_11_0,
		__uuidof(ID3D12Device),
		(void**)&device
	));
	
	device->QueryInterface(IID_PPV_ARGS(&debug_device));

	D3D12_COMMAND_QUEUE_DESC command_queue_description = { };
	command_queue_description.Type = D3D12_COMMAND_LIST_TYPE::D3D12_COMMAND_LIST_TYPE_DIRECT;
	command_queue_description.Priority = D3D12_COMMAND_QUEUE_PRIORITY::D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	command_queue_description.Flags = D3D12_COMMAND_QUEUE_FLAGS::D3D12_COMMAND_QUEUE_FLAG_NONE;
	command_queue_description.NodeMask = 0;

	// Create the command queue.
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(device->CreateCommandQueue(&command_queue_description, __uuidof(ID3D12CommandQueue), (void**)&command_queue));

	command_queue->QueryInterface(IID_PPV_ARGS(&debug_command_queue));

	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(CreateDXGIFactory1(__uuidof(IDXGIFactory4), (void**)&factory));
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(factory->EnumAdapters(0, &adapter));
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(adapter->EnumOutputs(0, &adapter_output));

	sample_description.Count = 1;
	sample_description.Quality = 0;

	DXGI_SWAP_CHAIN_DESC swap_chain_description = { };
	swap_chain_description.BufferDesc.Width = resolution.x;
	swap_chain_description.BufferDesc.Height = resolution.y;
	swap_chain_description.BufferDesc.Format = DXGI_FORMAT::DXGI_FORMAT_R8G8B8A8_UNORM; // Or DXGI_FORMAT_B8G8R8A8_UNORM? // Layout of pixels
	swap_chain_description.BufferDesc.RefreshRate.Numerator = 0; // Pick whatever is already there
	swap_chain_description.BufferDesc.RefreshRate.Denominator = 0; // Pick whatever is already there
	swap_chain_description.BufferDesc.Scaling = DXGI_MODE_SCALING::DXGI_MODE_SCALING_UNSPECIFIED; // Don't need scaling
	swap_chain_description.BufferDesc.ScanlineOrdering = DXGI_MODE_SCANLINE_ORDER::DXGI_MODE_SCANLINE_ORDER_UNSPECIFIED; // Scanline order (Interlaced, etc)
	swap_chain_description.SampleDesc = sample_description;
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

	command_queue->QueryInterface(IID_PPV_ARGS(&debug_command_queue));
	
	debug_interface->EnableDebugLayer();

	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(command_list->Close());

	// Create a fence for GPU synchronization.
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&fence));
	// Create an event object for the fence.
	fence_event = CreateEventExW(NULL, FALSE, FALSE, EVENT_ALL_ACCESS);

	CD3DX12_ROOT_SIGNATURE_DESC root_signature_description;
	root_signature_description.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ComPtr<ID3DBlob> signature;
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(D3D12SerializeRootSignature(&root_signature_description, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr));
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&root_signature)));

	// create vertex and pixel shaders

	// when debugging, we can compile the shader files at runtime.
	// but for release versions, we can compile the hlsl shaders
	// with fxc.exe to create .cso files, which contain the shader
	// bytecode. We can load the .cso files at runtime to get the
	// shader bytecode, which of course is faster than compiling
	// them at runtime

	// compile vertex shader
	ID3DBlob* vertexShader; // d3d blob for holding vertex shader bytecode
	ID3DBlob* errorBuff; // a buffer holding the error data if any
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(D3DCompileFromFile(L"VertexShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertexShader,
		&errorBuff
	));

	// fill out a shader bytecode structure, which is basically just a pointer
	// to the shader bytecode and the size of the shader bytecode
	D3D12_SHADER_BYTECODE vertexShaderBytecode = {};
	vertexShaderBytecode.BytecodeLength = vertexShader->GetBufferSize();
	vertexShaderBytecode.pShaderBytecode = vertexShader->GetBufferPointer();

	// compile pixel shader
	ID3DBlob* pixelShader;
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(D3DCompileFromFile(L"PixelShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixelShader,
		&errorBuff
	));

	// fill out shader bytecode structure for pixel shader
	D3D12_SHADER_BYTECODE pixelShaderBytecode = {};
	pixelShaderBytecode.BytecodeLength = pixelShader->GetBufferSize();
	pixelShaderBytecode.pShaderBytecode = pixelShader->GetBufferPointer();

	D3D12_INPUT_ELEMENT_DESC input_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
		//{ "COLOR", 0, DXGI_FORMAT_R32G32B32A32_FLOAT, 0, 12, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// fill out an input layout description structure
	D3D12_INPUT_LAYOUT_DESC inputLayoutDesc = {};

	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	inputLayoutDesc.NumElements = sizeof(input_layout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	inputLayoutDesc.pInputElementDescs = input_layout;

	// create a pipeline state object (PSO)

	// In a real application, you will have many pso's. for each different shader
	// or different combinations of shaders, different blend states or different rasterizer states,
	// different topology types (point, line, triangle, patch), or a different number
	// of render targets you will need a pso

	// VS is the only required shader for a pso. You might be wondering when a case would be where
	// you only set the VS. It's possible that you have a pso that only outputs data with the stream
	// output, and not on a render target, which means you would not need anything after the stream
	// output.

	D3D12_GRAPHICS_PIPELINE_STATE_DESC psoDesc = {}; // a structure to define a pso
	psoDesc.InputLayout = inputLayoutDesc; // the structure describing our input layout
	psoDesc.pRootSignature = root_signature.Get(); // the root signature that describes the input data this pso needs
	psoDesc.VS = vertexShaderBytecode; // structure describing where to find the vertex shader bytecode and how large it is
	psoDesc.PS = pixelShaderBytecode; // same as VS but for pixel shader
	psoDesc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	psoDesc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	psoDesc.SampleDesc = sample_description; // must be the same sample description as the swapchain and depth/stencil buffer
	psoDesc.SampleMask = UINT_MAX; // sample mask has to do with multi-sampling. UINT_MAX (0xffffffff) means point sampling is done
	psoDesc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
	psoDesc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	psoDesc.NumRenderTargets = 1; // we are only binding one render target

	// create the pso
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(device->CreateGraphicsPipelineState(&psoDesc, IID_PPV_ARGS(&pipeline_state_object)));

	// Create vertex buffer

	// a triangle
	SimpleVertex vList[] = {
		{ { 0.0f, 0.5f, 0.5f } },
		{ { 0.5f, -0.5f, 0.5f } },
		{ { -0.5f, -0.5f, 0.5f } },
	};

	int vBufferSize = sizeof(vList);

	// create default heap
	// default heap is memory on the GPU. Only the GPU has access to this memory
	// To get data into this heap, we will have to upload the data using
	// an upload heap

	auto heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto buffer = CD3DX12_RESOURCE_DESC::Buffer(vBufferSize);
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(device->CreateCommittedResource(
		&heap, // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&buffer, // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
										// from the upload heap to this heap
		nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&vertex_buffer)));

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(vertex_buffer->SetName(L"Vertex Buffer Resource Heap"));

	// create upload heap
	// upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
	// We will upload the vertex buffer using this heap to the default heap
	ID3D12Resource* vBufferUploadHeap;

	heap = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	buffer = CD3DX12_RESOURCE_DESC::Buffer(vBufferSize);
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(device->CreateCommittedResource(
		&heap, // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&buffer, // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vBufferUploadHeap)));
	HANDLE_POSSIBLE_EXCEPTION_WINDOWS(vBufferUploadHeap->SetName(L"Vertex Buffer Upload Resource Heap"));

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA vertexData = {};
	vertexData.pData = reinterpret_cast<BYTE*>(vList); // pointer to our vertex array
	vertexData.RowPitch = vBufferSize; // size of all our triangle vertex data
	vertexData.SlicePitch = vBufferSize; // also the size of our triangle vertex data

										 // we are now creating a command with the command list to copy the data from
										 // the upload heap to the default heap
	UpdateSubresources(command_list.Get(), vertex_buffer.Get(), vBufferUploadHeap, 0, 0, 1, &vertexData);

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
	vertex_buffer_view.StrideInBytes = sizeof(Vertex);
	vertex_buffer_view.SizeInBytes = vBufferSize;

	UVector2 window_size = get_window_size(window);

	// Fill out the Viewport
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = window_size.x;
	viewport.Height = window_size.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	scissor_rect.left = 0;
	scissor_rect.top = 0;
	scissor_rect.right = window_size.x;
	scissor_rect.bottom = window_size.y;
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
