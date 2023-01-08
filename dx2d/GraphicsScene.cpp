#include "GraphicsScene.h"

GraphicsScene::GraphicsScene(HWND window, const ObjectVector &objects)
	: window(window), objects(objects) {
	initD3D();
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
	DXGI_MODE_DESC back_buffer_desc = {}; // this is to describe our display mode
	back_buffer_desc.Width = resolution.x; // buffer width
	back_buffer_desc.Height = resolution.y; // buffer height
	back_buffer_desc.Format = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the buffer (rgba 32 bits, 8 bits for each chanel)

	// describe our multi-sampling. We are not multi-sampling, so we set the count to 1 (we need at least one sample of course)
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
}

void GraphicsScene::create_back_buffers_and_rtv_with_descriptor_heap() {
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

	
}

void GraphicsScene::create_command_allocators() {
	
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		HPEW(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&command_allocator[i])));
	}
	
}

void GraphicsScene::create_command_list() {
	// create the command list with the first allocator
	HPEW(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, command_allocator[0].Get(), NULL, IID_PPV_ARGS(&command_list)));

	command_list->QueryInterface(IID_PPV_ARGS(&debug_command_list));
}

void GraphicsScene::create_fence_and_fence_event() {
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

void GraphicsScene::create_root_signature() {
	CD3DX12_ROOT_SIGNATURE_DESC root_signature_desc;
	root_signature_desc.Init(0, nullptr, 0, nullptr, D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

	ID3DBlob *signature;
	HPEW(D3D12SerializeRootSignature(&root_signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature, nullptr));

	HPEW(device->CreateRootSignature(0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&root_signature)));
}

void GraphicsScene::create_vertex_and_pixel_shaders() {
	// when debugging, we can compile the shader files at runtime.
	// but for release versions, we can compile the hlsl shaders
	// with fxc.exe to create .cso files, which contain the shader
	// bytecode. We can load the .cso files at runtime to get the
	// shader bytecode, which of course is faster than compiling
	// them at runtime

	// compile vertex shader
	ID3DBlob* vertex_shader; // d3d blob for holding vertex shader bytecode
	ID3DBlob* error_buff; // a buffer holding the error data if any
	HPEW(D3DCompileFromFile(L"VertexShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"vs_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&vertex_shader,
		&error_buff
	), (error_buff == nullptr ? "" : (char*)error_buff->GetBufferPointer()));

	// fill out a shader bytecode structure, which is basically just a pointer
	// to the shader bytecode and the size of the shader bytecode
	vs_bytecode.BytecodeLength = vertex_shader->GetBufferSize();
	vs_bytecode.pShaderBytecode = vertex_shader->GetBufferPointer();

	// compile pixel shader
	ID3DBlob* pixel_shader;
	HPEW(D3DCompileFromFile(L"PixelShader.hlsl",
		nullptr,
		nullptr,
		"main",
		"ps_5_0",
		D3DCOMPILE_DEBUG | D3DCOMPILE_SKIP_OPTIMIZATION,
		0,
		&pixel_shader,
		&error_buff
	), (error_buff == nullptr ? "" : (char*)error_buff->GetBufferPointer()));

	// fill out shader bytecode structure for pixel shader
	ps_bytecode.BytecodeLength = pixel_shader->GetBufferSize();
	ps_bytecode.pShaderBytecode = pixel_shader->GetBufferPointer();
}

void GraphicsScene::create_pso() {
	constexpr D3D12_INPUT_ELEMENT_DESC input_layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0,                            D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT,    0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "NORMAL",   0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TANGENT",  0, DXGI_FORMAT_R32G32B32_FLOAT, 0, D3D12_APPEND_ALIGNED_ELEMENT, D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 }
	};

	// fill out an input layout description structure
	D3D12_INPUT_LAYOUT_DESC input_layout_desc = {};

	// we can get the number of elements in an array by "sizeof(array) / sizeof(arrayElementType)"
	input_layout_desc.NumElements = sizeof(input_layout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	input_layout_desc.pInputElementDescs = input_layout;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {}; // a structure to define a pso
	pso_desc.InputLayout = input_layout_desc; // the structure describing our input layout
	pso_desc.pRootSignature = root_signature.Get(); // the root signature that describes the input data this pso needs
	pso_desc.VS = vs_bytecode; // structure describing where to find the vertex shader bytecode and how large it is
	pso_desc.PS = ps_bytecode; // same as VS but for pixel shader
	pso_desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE; // type of topology we are drawing
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	pso_desc.SampleDesc = sample_desc; // must be the same sample description as the swapchain and depth/stencil buffer
	pso_desc.SampleMask = UINT_MAX; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	pso_desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT); // a default rasterizer state.
	pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	pso_desc.NumRenderTargets = 1; // we are only binding one render target
	pso_desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT); // a default depth stencil state
	pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;

	// create the pso
	HPEW(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipeline_state_object)));
}

void GraphicsScene::create_depth_stencil() {
	// create a depth stencil descriptor heap so we can get a pointer to the depth stencil buffer
	D3D12_DESCRIPTOR_HEAP_DESC dsv_heap_desc = {};
	dsv_heap_desc.NumDescriptors = 1;
	dsv_heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	dsv_heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HPEW(device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(&depth_stencil_descriptor_heap)));

	D3D12_DEPTH_STENCIL_VIEW_DESC depth_stencil_desc = {};
	depth_stencil_desc.Format = DXGI_FORMAT_D32_FLOAT;
	depth_stencil_desc.ViewDimension = D3D12_DSV_DIMENSION_TEXTURE2D;
	depth_stencil_desc.Flags = D3D12_DSV_FLAG_NONE;

	D3D12_CLEAR_VALUE depth_optimized_clear_value = {};
	depth_optimized_clear_value.Format = DXGI_FORMAT_D32_FLOAT;
	depth_optimized_clear_value.DepthStencil.Depth = 1.0f;
	depth_optimized_clear_value.DepthStencil.Stencil = 0;

	auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto tex = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, resolution.x, resolution.y, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	device->CreateCommittedResource(
		&heap_properties,
		D3D12_HEAP_FLAG_NONE,
		&tex,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depth_optimized_clear_value,
		IID_PPV_ARGS(&depth_stencil_buffer)
	);
	HPEW(device->CreateDescriptorHeap(&dsv_heap_desc, IID_PPV_ARGS(&depth_stencil_descriptor_heap)));
	depth_stencil_descriptor_heap->SetName(L"Depth/Stencil Resource Heap");

	device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_desc, depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart());
}

void GraphicsScene::create_vertex_buffer() {
	// a triangle
	Vertex verts[] = {
		Vertex(0.0f, 0.5f, 0.5f),
		Vertex(0.5f, -0.5f, 0.5f),
		Vertex(-0.5f, -0.5f, 0.5f),
	};

	int vBufferSize = sizeof(verts);

	// create default heap
	// default heap is memory on the GPU. Only the GPU has access to this memory
	// To get data into this heap, we will have to upload the data using
	// an upload heap
	auto buffer = CD3DX12_RESOURCE_DESC::Buffer(vBufferSize);
	auto heap_properties1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	device->CreateCommittedResource(
		&heap_properties1, // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&buffer, // resource description for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
										// from the upload heap to this heap
		nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&vertex_buffer));

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	vertex_buffer->SetName(L"Vertex Buffer Resource Heap");

	// create upload heap
	// upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
	// We will upload the vertex buffer using this heap to the default heap
	auto heap_properties2 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto buffer2 = CD3DX12_RESOURCE_DESC::Buffer(vBufferSize);
	device->CreateCommittedResource(
		&heap_properties2, // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&buffer2, // resource description for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vertex_buffer_upload_heap));
	vertex_buffer_upload_heap->SetName(L"Vertex Buffer Upload Resource Heap");

	// store vertex buffer in upload heap
	D3D12_SUBRESOURCE_DATA vertex_data = {};
	vertex_data.pData = reinterpret_cast<BYTE*>(verts); // pointer to our vertex array
	vertex_data.RowPitch = vBufferSize; // size of all our triangle vertex data
	vertex_data.SlicePitch = vBufferSize; // also the size of our triangle vertex data

	// we are now creating a command with the command list to copy the data from
	// the upload heap to the default heap
	UpdateSubresources(command_list.Get(), vertex_buffer.Get(), vertex_buffer_upload_heap.Get(), 0, 0, 1, &vertex_data);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	auto transition = CD3DX12_RESOURCE_BARRIER::Transition(vertex_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	command_list->ResourceBarrier(1, &transition);

	// Now we execute the command list to upload the initial assets (triangle data)
	command_list->Close();
	ID3D12CommandList* ppCommandLists[] = { command_list.Get()};
	command_queue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);

	// increment the fence value now, otherwise the buffer might not be uploaded by the time we start drawing
	fence_value[frame_index]++;
	HPEW(command_queue->Signal(fence[frame_index].Get(), fence_value[frame_index]));

	// create a vertex buffer view for the triangle. We get the GPU memory address to the vertex pointer using the GetGPUVirtualAddress() method
	vertex_buffer_view.BufferLocation = vertex_buffer->GetGPUVirtualAddress();
	vertex_buffer_view.StrideInBytes = sizeof(Vertex);
	vertex_buffer_view.SizeInBytes = vBufferSize;
}

void GraphicsScene::fill_out_om_viewing_settings() {
	// Fill out the Viewport
	viewport.TopLeftX = 0;
	viewport.TopLeftY = 0;
	viewport.Width = (float)resolution.x;
	viewport.Height = (float)resolution.y;
	viewport.MinDepth = 0.0f;
	viewport.MaxDepth = 1.0f;

	// Fill out a scissor rect
	scissor_rect.left = 0;
	scissor_rect.top = 0;
	scissor_rect.right = resolution.x;
	scissor_rect.bottom = resolution.y;
}

void GraphicsScene::initD3D() {
	HPEW(D3D12GetDebugInterface(IID_PPV_ARGS(&debug_interface)));
	debug_interface->EnableDebugLayer();

	create_adapter_and_device();
	create_command_queue();
	create_swap_chain();
	create_back_buffers_and_rtv_with_descriptor_heap();
	create_command_allocators();
	create_command_list();
	create_fence_and_fence_event();
	create_root_signature();
	create_vertex_and_pixel_shaders();
	create_pso();
	create_depth_stencil();
	create_vertex_buffer();
	fill_out_om_viewing_settings();
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

	HPEW(command_list->Reset(command_allocator[frame_index].Get(), pipeline_state_object.Get()));

	// here we start recording commands into the commandList (which all the commands will be stored in the commandAllocator)

	// transition the "frameIndex" render target from the present state to the render target state so the command list draws to it starting from here
	CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(render_targets[frame_index].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	command_list->ResourceBarrier(1, &barrier);

	// here we again get the handle to our current render target view so we can set it as the render target in the output merger stage of the pipeline
	CD3DX12_CPU_DESCRIPTOR_HANDLE rtv_handle(rtv_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), frame_index, rtv_descriptor_size);
	// get a handle to the depth/stencil buffer
	CD3DX12_CPU_DESCRIPTOR_HANDLE dsv_handle(depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart());

	// set the render target for the output merger stage (the output of the pipeline)
	command_list->OMSetRenderTargets(1, &rtv_handle, FALSE, &dsv_handle);

	// Clear the render target by using the ClearRenderTargetView command
	const float color[4] = { background_color.r, background_color.g,
							 background_color.b, background_color.a };
	command_list->ClearRenderTargetView(rtv_handle, color, 0, nullptr);

	// clear the depth/stencil buffer
	command_list->ClearDepthStencilView(depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

	// draw triangle
	command_list->SetPipelineState(pipeline_state_object.Get());
	command_list->SetGraphicsRootSignature(root_signature.Get()); // set the root signature
	command_list->RSSetViewports(1, &viewport); // set the viewports
	command_list->RSSetScissorRects(1, &scissor_rect); // set the scissor rects
	command_list->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST); // set the primitive topology
	command_list->IASetVertexBuffers(0, 1, &vertex_buffer_view); // set the vertex buffer (using the vertex buffer view)
	command_list->DrawInstanced(3, 1, 0, 0); // finally draw 3 vertices (draw the triangle)

	// transition the "frameIndex" render target from the render target state to the present state. If the debug layer is enabled, you will receive a
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
