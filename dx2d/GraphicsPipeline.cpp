#include "GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline(ComPtr<ID3D12Device> &device, const DXGI_SAMPLE_DESC &sample_desc,
	const UVector2 &resolution) : rasterizer(Rasterizer(resolution)), output_merger(OutputMerger(device, resolution)) { }

void GraphicsPipeline::update(const ComPtr<ID3D12Device> &device, ComPtr<ID3D12GraphicsCommandList> &command_list, const CD3DX12_CPU_DESCRIPTOR_HANDLE &rtv_handle, int frame_index) {
	command_list->SetPipelineState(pipeline_state_object.Get());
	command_list->SetGraphicsRootSignature(root_signature.signature.Get()); // set the root signature
	
	root_signature.update(device, command_list, frame_index);
	input_assembler.update(command_list);
	rasterizer.update(command_list);
	output_merger.update(command_list, rtv_handle);
}

void GraphicsPipeline::run(const ComPtr<ID3D12Device> &device, ComPtr<ID3D12GraphicsCommandList> &command_list, const CD3DX12_CPU_DESCRIPTOR_HANDLE &rtv_handle, int frame_index) {
	update(device, command_list, rtv_handle, frame_index);
	
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertex_buffers = input_assembler.get_vertex_buffer_views();
	UINT verts = 0;
	for (const D3D12_VERTEX_BUFFER_VIEW &view : vertex_buffers) {
		verts += view.SizeInBytes/sizeof(Vertex);
	}

	command_list->DrawInstanced(verts, 1, 0, 0);
}

void GraphicsPipeline::compile(ComPtr<ID3D12Device> &device, const DXGI_SAMPLE_DESC &sample_desc, const UVector2 &resolution) {
	root_signature.compile(device);
	vs.compile();
	hs.compile();
	ds.compile();
	gs.compile();
	rasterizer.compile(resolution);
	ps.compile();
	output_merger.compile(resolution, device);

	// Fill PSO

	// fill out an input layout desc structure
	D3D12_INPUT_LAYOUT_DESC input_layout_desc = {};

	input_layout_desc.NumElements = sizeof(input_layout) / sizeof(D3D12_INPUT_ELEMENT_DESC);
	input_layout_desc.pInputElementDescs = input_layout;

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {}; // a structure to define a pso
	pso_desc.pRootSignature = root_signature.signature.Get(); // the root signature that describes the input data this pso needs
	pso_desc.InputLayout = input_layout_desc; // the structure describing our input layout
	pso_desc.PrimitiveTopologyType = input_assembler.primitive_topology_type; // type of topology we are drawing
	pso_desc.VS = vs.bytecode; // structure describing where to find the vertex shader bytecode and how large it is
	pso_desc.HS = hs.bytecode;
	pso_desc.DS = ds.bytecode;
	pso_desc.StreamOutput = stream_output.desc;
	pso_desc.GS = gs.bytecode;
	pso_desc.RasterizerState = rasterizer.desc;
	pso_desc.PS = ps.bytecode;
	pso_desc.DepthStencilState = output_merger.depth_stencil_desc; // a default depth stencil state
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	pso_desc.SampleDesc = sample_desc; // must be the same sample desc as the swapchain and depth/stencil buffer
	pso_desc.SampleMask = UINT_MAX; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blent state.
	pso_desc.NumRenderTargets = 1; // we are only binding one render target
	pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//pso_desc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;

	// create the pso
	HPEW(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipeline_state_object)));
}

void GraphicsPipeline::clean_up() {
	SAFE_RELEASE(output_merger.depth_stencil_buffer);
	SAFE_RELEASE(output_merger.depth_stencil_descriptor_heap);
	for (auto &buffer : input_assembler.vertex_buffers) {
		SAFE_RELEASE(buffer);
	}
}

bool GraphicsPipeline::operator==(const GraphicsPipeline &pipeline) const noexcept {
	return (pipeline_state_object == pipeline.pipeline_state_object &&
	root_signature == pipeline.root_signature &&
	input_assembler == pipeline.input_assembler &&

	vs == pipeline.vs &&
	hs == pipeline.hs &&
	ds == pipeline.ds &&
	gs == pipeline.gs &&
	ps == pipeline.ps &&

	tesselator == pipeline.tesselator &&
	rasterizer == pipeline.rasterizer &&
	output_merger == pipeline.output_merger &&
	stream_output == pipeline.stream_output);
}

void GraphicsPipeline::operator=(const GraphicsPipeline &pipeline) noexcept {
	pipeline_state_object = pipeline.pipeline_state_object;
	root_signature = pipeline.root_signature;
	input_assembler = pipeline.input_assembler;
	
	vs = pipeline.vs;
	hs = pipeline.hs;
	ds = pipeline.ds;
	gs = pipeline.gs;
	ps = pipeline.ps;

	tesselator = pipeline.tesselator;
	rasterizer = pipeline.rasterizer;
	output_merger = pipeline.output_merger;
	stream_output = pipeline.stream_output;
}

void GraphicsPipeline::InputAssembler::add_mesh(const MeshData &mesh, ComPtr<ID3D12Device> &device,
	ComPtr<ID3D12GraphicsCommandList> &command_list, size_t index) {

	const std::vector<Vertex> &verts = mesh.get_vertices();

	if (index == (size_t)-1) {
		index = vertex_buffers.size();
	}

	ID3D12Resource* vertex_buffer_upload = nullptr;
	auto upload_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto upload_buffer = CD3DX12_RESOURCE_DESC::Buffer(verts.size() * sizeof(Vertex));
	device->CreateCommittedResource(
		&upload_heap_properties, // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&upload_buffer, // resource desc for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vertex_buffer_upload));

	vertex_buffer_upload->SetName(L"Vertex Buffer Upload Resource Heap");

	void* p = nullptr;
	vertex_buffer_upload->Map(0, nullptr, &p);
	memcpy(p, &verts[0], sizeof(Vertex) * verts.size());
	vertex_buffer_upload->Unmap(0, nullptr);

	vertex_buffers.insert(vertex_buffers.begin()+index, nullptr);
	vertex_buffer_views.insert(vertex_buffer_views.begin()+index, D3D12_VERTEX_BUFFER_VIEW());

	auto default_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto default_buffer = CD3DX12_RESOURCE_DESC::Buffer(verts.size()*sizeof(Vertex));
	device->CreateCommittedResource(
		&default_heap_properties, // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&default_buffer, // resource desc for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
		// from the upload heap to this heap
		nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&vertex_buffers[index])
	);

	vertex_buffers[index]->SetName(L"Vertex Buffer Default Resource Heap");

	command_list->CopyResource(vertex_buffers[index].Get(), vertex_buffer_upload);

	vertex_buffer_views[index].BufferLocation = vertex_buffers[index]->GetGPUVirtualAddress();
	vertex_buffer_views[index].SizeInBytes = (UINT)verts.size() * sizeof(Vertex);
	vertex_buffer_views[index].StrideInBytes = sizeof(Vertex);

	// we can give resource heaps a name so when we debug with the graphics debugger we know what resource we are looking at
	vertex_buffers[index]->SetName(L"Vertex Buffer Default Resource Heap");

	// transition the vertex buffer data from copy destination state to vertex buffer state
	auto transition = CD3DX12_RESOURCE_BARRIER::Transition(vertex_buffers[index].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	command_list->ResourceBarrier(1, &transition);
}

void GraphicsPipeline::InputAssembler::remove_mesh(size_t index) {
	vertex_buffers.erase(vertex_buffers.begin() + index);
}

const std::vector<D3D12_VERTEX_BUFFER_VIEW> & GraphicsPipeline::InputAssembler::get_vertex_buffer_views() const noexcept {
	return vertex_buffer_views;
}

void GraphicsPipeline::OutputMerger::create_depth_stencil(const UVector2 &resolution, ComPtr<ID3D12Device> &device) {
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

	D3D12_CLEAR_VALUE depth_clear_value = {};
	depth_clear_value.Format = DXGI_FORMAT_D32_FLOAT;
	depth_clear_value.DepthStencil.Depth = 1.0f;
	depth_clear_value.DepthStencil.Stencil = 0;

	auto heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto tex = CD3DX12_RESOURCE_DESC::Tex2D(DXGI_FORMAT_D32_FLOAT, resolution.x, resolution.y, 1, 1, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	device->CreateCommittedResource(
		&heap_properties,
		D3D12_HEAP_FLAG_NONE,
		&tex,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depth_clear_value,
		IID_PPV_ARGS(&depth_stencil_buffer)
	);

	depth_stencil_buffer->SetName(L"Depth Stencil Buffer");

	device->CreateDepthStencilView(depth_stencil_buffer.Get(), &depth_stencil_desc, depth_stencil_descriptor_heap->GetCPUDescriptorHandleForHeapStart());
}

void GraphicsPipeline::RootSignature::compile(ComPtr<ID3D12Device> &device) {
	std::vector<D3D12_ROOT_PARAMETER> params;
	for (const DescriptorTable &table : descriptor_tables) {
		params.push_back(table.root_parameters[0]);
	}

	signature_desc.Init((UINT)params.size(), (params.empty() ? nullptr : &params[0]), 0, nullptr,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ComPtr<ID3DBlob> signature_blob;
	ComPtr<ID3DBlob> error_buf;
	HPEW(D3D12SerializeRootSignature(&signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &error_buf), HPEW_ERR_BLOB_PARAM(error_buf));

	HPEW(device->CreateRootSignature(0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&signature)));

	// Create a constant buffer descriptor heap for each frame
	// this is the descriptor heap that will store our constant buffer descriptor
	if (!descriptor_tables.empty()) {
		for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
			D3D12_DESCRIPTOR_HEAP_DESC heap_desc = {};
			heap_desc.NumDescriptors = (UINT)descriptor_tables.size();
			heap_desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
			heap_desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
			heap_desc.NodeMask = 0u;

			HPEW(device->CreateDescriptorHeap(&heap_desc, IID_PPV_ARGS(&descriptor_heaps[i])));
		}
	}

	for (ConstantBuffer* cb : constant_buffers) {
		cb->compile(device, descriptor_heaps);
	}
}

void GraphicsPipeline::RootSignature::update(const ComPtr<ID3D12Device> &device, ComPtr<ID3D12GraphicsCommandList> &command_list, int frame_index) {
	for (ConstantBuffer* cb : constant_buffers) {
		cb->apply(frame_index);
	}
	
	// set constant buffer descriptor heap
	ID3D12DescriptorHeap* heaps[] = { descriptor_heaps[frame_index].Get() };
	if (!descriptor_tables.empty()) { // descriptor_heaps is only empty when descriptor_tables is as well
		command_list->SetDescriptorHeaps(_countof(heaps), heaps);
	}

	int i = 0;
	for (const DescriptorTable &descriptor_table : descriptor_tables) {
		D3D12_GPU_DESCRIPTOR_HANDLE handle = { };
		handle.ptr = descriptor_heaps[frame_index]->GetGPUDescriptorHandleForHeapStart().ptr + descriptor_table.ranges[0].BaseShaderRegister * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		command_list->SetGraphicsRootDescriptorTable(i++, handle);
	}

	for (const RootConstants &constants : root_constants) {
		command_list->SetGraphicsRoot32BitConstants(constants.parameter_index, constants.obj_size / 32u, constants.obj, 0u);
	}
}

bool GraphicsPipeline::RootSignature::operator==(const RootSignature &root_signature) const noexcept {
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		if (descriptor_heaps[i] == root_signature.descriptor_heaps[i]) return false;
	}

	return (signature == root_signature.signature &&
		descriptor_tables == root_signature.descriptor_tables &&
		constant_buffers == root_signature.constant_buffers &&
		signature_desc == root_signature.signature_desc);
}

void GraphicsPipeline::RootSignature::bind_constant_buffer(ConstantBuffer &cb, D3D12_SHADER_VISIBILITY shader) {
	UINT index = (UINT)constant_buffers.size() + (UINT)root_constants.size();
	descriptor_tables.push_back(DescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, shader, index, index));
	cb.index = index;
	constant_buffers.push_back(&cb);
}

GraphicsPipeline::RootSignature::RootArgument::RootArgument(UINT parameter_index)
	: root_parameters(std::shared_ptr<D3D12_ROOT_PARAMETER[]>(new D3D12_ROOT_PARAMETER[PARAMS_SIZE])),
	parameter_index(parameter_index) { }

GraphicsPipeline::RootSignature::DescriptorTable::DescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_SHADER_VISIBILITY shader, UINT index, UINT parameter_index)
	: ranges(std::shared_ptr<D3D12_DESCRIPTOR_RANGE[]>(new D3D12_DESCRIPTOR_RANGE[RANGES_SIZE])),
	RootArgument(parameter_index) {
	compile(type, shader, index);
}

void GraphicsPipeline::RootSignature::DescriptorTable::compile(D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_SHADER_VISIBILITY shader, UINT index) {
	ranges[0].RangeType = type;
	ranges[0].NumDescriptors = 1;
	ranges[0].BaseShaderRegister = index;
	ranges[0].RegisterSpace = 0; // space 0. can usually be zero
	ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables
	
	table.NumDescriptorRanges = RANGES_SIZE;
	table.pDescriptorRanges = ranges.get();
	
	root_parameters[0].ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE; // this is a descriptor table
	root_parameters[0].DescriptorTable = table; // this is our descriptor table for this root parameter
	root_parameters[0].ShaderVisibility = shader;
}

bool GraphicsPipeline::RootSignature::DescriptorTable::operator==(const DescriptorTable &descriptor_table) const noexcept {
	return (table == descriptor_table.table &&
		ranges[0] == descriptor_table.ranges[0]);
}

void GraphicsPipeline::RootSignature::ConstantBuffer::apply(int frame_index) noexcept {
	memcpy(gpu_addresses[frame_index], obj, obj_size);
}

bool GraphicsPipeline::RootSignature::ConstantBuffer::operator==(const ConstantBuffer &cb) const noexcept {
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		if (gpu_addresses[i] != cb.gpu_addresses[i] && upload_heaps[i] == cb.upload_heaps[i]) return false;
	}
	
	return (//obj == cb.obj &&						std::equal_to<void> no matching call operator found
		obj_size == cb.obj_size &&
		name == cb.name);
}

// -- DYNAMIC MESHES -- //

/*
#include "ResourceManager.h"

ResourceManager::ResourceManager() { }

size_t ResourceManager::create_vertex_default_buffers(size_t number_of_verts, ComPtr<ID3D12Device> &device) {
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

void ResourceManager::fill_vertex_buffers(const std::vector<Vertex> &verts, size_t num_bufs, ComPtr<ID3D12Device> &device,
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

void ResourceManager::update_vertex_buffers(const std::vector<Vertex> &verts,
	ComPtr<ID3D12Device> &device, ComPtr<ID3D12GraphicsCommandList> &command_list) {
	const auto leftover_verts_in_buffer = BUFFER_VERTEX_COUNT_LIMIT - vertex_buffer_it.second;
	const UINT number_of_new_buffers_needed = create_vertex_default_buffers(verts.size(), device);
	
	fill_vertex_buffers(verts, create_vertex_default_buffers(verts.size(), device), device, command_list);
}

void ResourceManager::create_depth_stencil(const UVector2 &resolution, ComPtr<ID3D12Device> &device) {
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

void ResourceManager::clean_up() {
	SAFE_RELEASE(depth_stencil_buffer);
	SAFE_RELEASE(depth_stencil_descriptor_heap);
	for (auto &buffer : vertex_buffers) {
		SAFE_RELEASE(buffer.first);
	}
}

*/
