#include "GraphicsPipeline.h"

void GraphicsPipeline::check_for_update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_BLEND_DESC &blend_desc, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	if (compile_signal) {
		compile(device, command_list, sample_desc, blend_desc, descriptor_heaps);
	}
	
	input_assembler.check_for_update(device, command_list);
	root_signature.check_for_update(device, command_list, descriptor_heaps);
}

void GraphicsPipeline::run(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_BLEND_DESC &blend_desc, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	check_for_update(device, command_list, sample_desc, blend_desc, descriptor_heaps);

	command_list->SetPipelineState(pipeline_state_object.Get());

	input_assembler.run(command_list);
	root_signature.run(device, command_list, descriptor_heaps);
	draw(command_list);
}

void GraphicsPipeline::draw(const ComPtr<ID3D12GraphicsCommandList> &command_list) {
	input_assembler.draw_meshes(command_list);
}

void GraphicsPipeline::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_BLEND_DESC &blend_desc, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	input_assembler.compile(shaders.vs);
	root_signature.check_for_update(device, command_list, descriptor_heaps);
	root_signature.compile(device, command_list, descriptor_heaps, shaders);

	auto set_shader = [](D3D12_SHADER_BYTECODE& pso_bytecode, const std::weak_ptr<Shader>& shader) {
		if (!shader.expired()) {
			ComPtr<IDxcBlob> blob{shader.lock()->get_bytecode_blob()};
			pso_bytecode.pShaderBytecode = blob->GetBufferPointer();
			pso_bytecode.BytecodeLength = blob->GetBufferSize();
		}
	};

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc{};
	pso_desc.pRootSignature = root_signature.signature.Get();
	pso_desc.InputLayout = input_assembler.get_input_layout_desc();
	pso_desc.PrimitiveTopologyType = input_assembler.primitive_topology_type;
	set_shader(pso_desc.VS, this->shaders.vs);
	set_shader(pso_desc.HS, this->shaders.hs);
	set_shader(pso_desc.DS, this->shaders.ds);
	pso_desc.StreamOutput = stream_output.desc;
	set_shader(pso_desc.GS, this->shaders.gs);
	pso_desc.RasterizerState = rasterizer.desc;
	set_shader(pso_desc.PS, this->shaders.ps);
	pso_desc.DepthStencilState = depth_stencil_desc;
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM;
	pso_desc.SampleDesc = sample_desc; // Must be the same sample desc as the swapchain and depth/stencil buffer
	pso_desc.SampleMask = UINT_MAX;
	pso_desc.BlendState = blend_desc;
	pso_desc.NumRenderTargets = 1;
	pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//pso_desc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;

	HPEW(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipeline_state_object)));
	HPEW(pipeline_state_object->SetName(L"Main PSO"));

	compile_signal = false;
}

void GraphicsPipeline::clean_up() {
	pipeline_state_object.Reset();

	input_assembler.clean_up();
	root_signature.clean_up();
}

bool GraphicsPipeline::operator==(const GraphicsPipeline &pipeline) const noexcept {
	return (
		pipeline_state_object == pipeline.pipeline_state_object &&
		root_signature == pipeline.root_signature &&
		input_assembler == pipeline.input_assembler &&

		shaders.vs.lock() == pipeline.shaders.vs.lock() &&
		shaders.hs.lock() == pipeline.shaders.hs.lock() &&
		shaders.ds.lock() == pipeline.shaders.ds.lock() &&
		shaders.gs.lock() == pipeline.shaders.gs.lock() &&
		shaders.ps.lock() == pipeline.shaders.ps.lock() &&

		rasterizer == pipeline.rasterizer &&
		stream_output == pipeline.stream_output
	);
}

// -- DYNAMIC MESHES -- //

/*
#include "ResourceManager.h"

resource_manager->ResourceManager() { }

size_t resource_manager->create_vertex_default_buffers(size_t number_of_verts, ComPtr<ID3D12Device> &device) {
	const double p = static_cast<int32_t>(number_of_verts)-(BUFFER_VERTEX_COUNT_LIMIT - static_cast<int32_t>(vertex_buffer_it.second));
	const uint16_t number_of_new_buffers_needed = std::ceil(p/BUFFER_VERTEX_COUNT_LIMIT) + (vertex_buffers.size() ? 0 : 1);

	for (auto i = 0; i < number_of_new_buffers_needed; i++) {
		// create default heap
		// default heap is memory on the GPU. Only the GPU has access to this memory
		// To get data into this heap, we will have to upload the data using
		// an upload heap

		vertex_buffers.push_back(std::make_pair(nullptr, D3D12_VERTEX_BUFFER_VIEW()));

		auto buffer = CD3DX12_RESOURCE_DESC::Buffer(DEFAULT_BUFFER_MAX_SIZE);
		auto heap_properties1 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		device->CreateCommittedResource(
			&heap_properties1, // a default heap
			D3D12_HEAP_FLAG_NONE, // no flags
			&buffer, // resource desc for a buffer
			D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
											// from the upload heap to this heap
			nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
			IID_PPV_ARGS(&vertex_buffers[vertex_buffers.size()-1].first)
		);

		// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
		vertex_buffers[vertex_buffers.size()-1].first->SetName(L"Vertex Buffer Default Resource Heap");
	}
	return number_of_new_buffers_needed;
}

void resource_manager->fill_vertex_buffers(const std::vector<Vertex> &verts, size_t num_bufs, ComPtr<ID3D12Device> &device,
	ComPtr<ID3D12GraphicsCommandList> &command_list) {
	// create upload heap
	// upload heaps are used to upload data to the GPU. CPU can write to it, GPU can read from it
	// We will upload the vertex buffer using this heap to the default heap
	ID3D12Resource *vertex_buffer_upload = nullptr;

	auto heap_properties2 = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto buffer2 = CD3DX12_RESOURCE_DESC::Buffer(verts.size());
	device->CreateCommittedResource(
		&heap_properties2, // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&buffer2, // resource desc for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vertex_buffer_upload));

	vertex_buffer_upload->SetName(L"Vertex Buffer Upload Resource Heap");

	void *p = nullptr;
	vertex_buffer_upload->Map(0, nullptr, &p);
	memcpy(p, &verts[0], sizeof(Vertex) * verts.size());
	vertex_buffer_upload->Unmap(0, nullptr);


	size_t num_elements_copied{0};
	while (num_elements_copied < verts.size()) {

		size_t to_copy = std::min(BUFFER_VERTEX_COUNT_LIMIT-vertex_buffer_it.second, verts.size()-num_elements_copied);
		
		command_list->CopyBufferRegion(
			vertex_buffers[vertex_buffer_it.first].first.Get(),
			vertex_buffer_it.second * sizeof(Vertex),
			vertex_buffer_upload,
			num_elements_copied * sizeof(Vertex),
			to_copy
		);

		// transition the vertex buffer data from copy destination state to vertex buffer state
		auto transition = CD3DX12_RESOURCE_BARRIER::Transition(vertex_buffers[vertex_buffer_it.first].first.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
		command_list->ResourceBarrier(1, &transition);

		// Initialize the vertex buffer view.
		vertex_buffers[vertex_buffer_it.first].second.BufferLocation = vertex_buffers[vertex_buffer_it.first].first->GetGPUVirtualAddress();
		vertex_buffers[vertex_buffer_it.first].second.SizeInBytes = to_copy * sizeof(Vertex);
		vertex_buffers[vertex_buffer_it.first].second.StrideInBytes = sizeof(Vertex);

		auto prev_it_second = vertex_buffer_it.second;
		vertex_buffer_it.second = vertex_buffer_it.second+to_copy;
		vertex_buffer_it.second = vertex_buffer_it.second >= BUFFER_VERTEX_COUNT_LIMIT ? 0 : vertex_buffer_it.second;
		vertex_buffer_it.first += vertex_buffer_it.second < prev_it_second;
		num_elements_copied += to_copy;
	}
}

void resource_manager->update_vertex_buffers(const std::vector<Vertex> &verts,
	ComPtr<ID3D12Device> &device, ComPtr<ID3D12GraphicsCommandList> &command_list) {
	const auto leftover_verts_in_buffer = BUFFER_VERTEX_COUNT_LIMIT - vertex_buffer_it.second;
	const UINT number_of_new_buffers_needed = create_vertex_default_buffers(verts.size(), device);
	
	fill_vertex_buffers(verts, create_vertex_default_buffers(verts.size(), device), device, command_list);
}

void resource_manager->create_depth_stencil(const UVector2 &resolution, ComPtr<ID3D12Device> &device) {
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
	depth_stencil_descriptor_heap->SetName(L"Depth Stencil Resource Heap");

	device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_desc, depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart());
	depth_stencil_buffer->SetName(L"Depth Stencil Buffer");
}

void resource_manager->clean_up() {
	SAFE_RELEASE(depth_stencil_buffer);
	SAFE_RELEASE(depth_stencil_descriptor_heap);
	for (auto &buffer : vertex_buffers) {
		SAFE_RELEASE(buffer.first);
	}
}

*/
