#include "GraphicsPipeline.h"

GraphicsPipeline::GraphicsPipeline(const ComPtr<ID3D12Device> &device, const DXGI_SAMPLE_DESC &sample_desc,
	const UVector2 &resolution) : rasterizer(Rasterizer(resolution)) { }

void GraphicsPipeline::update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, int frame_index) {
	command_list->SetPipelineState(pipeline_state_object.Get());
	command_list->SetGraphicsRootSignature(root_signature.signature.Get()); // set the root signature
	
	root_signature.update(device, command_list, frame_index);
	input_assembler.update(device, command_list);
	rasterizer.update(command_list);
}

void GraphicsPipeline::run(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, int frame_index, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc, const UVector2 &resolution) {
	if (compilation_signal) {
		compile(device, command_list, sample_desc, depth_stencil_desc, resolution);
	}
	
	update(device, command_list, frame_index);
	
	std::vector<D3D12_VERTEX_BUFFER_VIEW> vertex_buffers = input_assembler.get_vertex_buffer_views();
	UINT verts = 0;
	for (const D3D12_VERTEX_BUFFER_VIEW &view : vertex_buffers) {
		verts += view.SizeInBytes/sizeof(Vertex);
	}

	command_list->DrawInstanced(verts, 1, 0, 0);
}

void GraphicsPipeline::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc, const UVector2 &resolution) {
	root_signature.compile(device, command_list);
	shader_storage->add_and_compile_shader(Shader::Type::Vertex, vs);
	shader_storage->add_and_compile_shader(Shader::Type::Hull, hs);
	shader_storage->add_and_compile_shader(Shader::Type::Domain, ds);
	shader_storage->add_and_compile_shader(Shader::Type::Geometry, gs);
	shader_storage->add_and_compile_shader(Shader::Type::Pixel, ps);
	rasterizer.compile(resolution);

	// Fill PSO

	// fill out an input layout desc structure
	D3D12_INPUT_LAYOUT_DESC input_layout_desc = {};

	input_layout_desc.NumElements = (UINT)input_layout.size();
	input_layout_desc.pInputElementDescs = &input_layout[0];

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {}; // a structure to define a pso
	pso_desc.pRootSignature = root_signature.signature.Get(); // the root signature that describes the input data this pso needs
	pso_desc.InputLayout = input_layout_desc; // the structure describing our input layout
	pso_desc.PrimitiveTopologyType = input_assembler.primitive_topology_type; // type of topology we are drawing
	pso_desc.VS = shader_storage->get_shader(vs)->get().get_bytecode(); // structure describing where to find the vertex shader bytecode and how large it is
	pso_desc.HS = shader_storage->get_shader(hs)->get().get_bytecode();
	pso_desc.DS = shader_storage->get_shader(ds)->get().get_bytecode();
	pso_desc.StreamOutput = stream_output.desc;
	pso_desc.GS = shader_storage->get_shader(gs)->get().get_bytecode();
	pso_desc.RasterizerState = rasterizer.desc;
	pso_desc.PS = shader_storage->get_shader(ps)->get().get_bytecode();
	pso_desc.DepthStencilState = depth_stencil_desc; // a default depth stencil state
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	pso_desc.SampleDesc = sample_desc; // must be the same sample desc as the swapchain and depth/stencil buffer
	pso_desc.SampleMask = UINT_MAX; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	pso_desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT); // a default blend state.
	pso_desc.NumRenderTargets = 1; // we are only binding one render target
	pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//pso_desc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;

	// create the pso
	HPEW(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipeline_state_object)));
	HPEW(pipeline_state_object->SetName(L"Main PSO"));
}

void GraphicsPipeline::clean_up() {
	input_assembler.change_manager.reset();
	for (auto &buffer : input_assembler.vertex_buffers) {
		buffer.Reset();
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

	rasterizer == pipeline.rasterizer &&
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

	rasterizer = pipeline.rasterizer;
	stream_output = pipeline.stream_output;
}

// +-----------------+
// | Input Assembler |
// +-----------------+

void GraphicsPipeline::InputAssembler::add_mesh(const Mesh &mesh, const ComPtr<ID3D12Device> &device,
	const ComPtr<ID3D12GraphicsCommandList> &command_list, size_t index) {

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

	ResourceManager::Release::resources.push_back(vertex_buffer_upload);
	HPEW(vertex_buffer_upload->SetName(L"Vertex Buffer Upload Resource Heap"));

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

	HPEW(vertex_buffers[index]->SetName(L"Vertex Buffer Default Resource Heap"));

	command_list->CopyResource(vertex_buffers[index].Get(), vertex_buffer_upload);

	vertex_buffer_views[index].BufferLocation = vertex_buffers[index]->GetGPUVirtualAddress();
	vertex_buffer_views[index].SizeInBytes = (UINT)verts.size() * sizeof(Vertex);
	vertex_buffer_views[index].StrideInBytes = sizeof(Vertex);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	auto transition = CD3DX12_RESOURCE_BARRIER::Transition(vertex_buffers[index].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	command_list->ResourceBarrier(1, &transition);
}

void GraphicsPipeline::InputAssembler::remove_mesh(size_t index) {
	ResourceManager::Release::resources.push_back(vertex_buffers[index]);
	vertex_buffers.erase(vertex_buffers.begin() + index);
	vertex_buffer_views.erase(vertex_buffer_views.begin() + index);
}

void GraphicsPipeline::InputAssembler::remove_all_meshes() {
	for (auto &buffer : vertex_buffers) {
		ResourceManager::Release::resources.push_back(buffer);
	}

	vertex_buffers.clear();
	vertex_buffer_views.clear();
}

void GraphicsPipeline::InputAssembler::update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list) {
	std::vector<GraphicsPipelineMeshChange::Command> changes = change_manager->get_changes(true);
	
	for (const auto &change : changes) {
		if (change.type == GraphicsPipelineMeshChange::Command::Type::Add) {
			std::pair<Mesh, size_t> addition = std::get<std::pair<Mesh, size_t>>(change.change);
			add_mesh(addition.first, device, command_list, addition.second);
		} else {
			size_t subtraction = std::get<size_t>(change.change);
			if (subtraction == (size_t)-1) {
				remove_all_meshes();
			} else {
				remove_mesh(subtraction);
			}
		}
	}

	command_list->IASetPrimitiveTopology(primitive_topology); // set the primitive topology
	command_list->IASetVertexBuffers(0, (UINT)vertex_buffer_views.size(), vertex_buffer_views.data()); // set the vertex buffer (using the vertex buffer view)
}

const std::vector<D3D12_VERTEX_BUFFER_VIEW> & GraphicsPipeline::InputAssembler::get_vertex_buffer_views() const noexcept {
	return vertex_buffer_views;
}

// +----------------+
// | Root Signature |
// +----------------+

void GraphicsPipeline::RootSignature::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list) {
	compilation_params.clear();
	for (int i = 0; i < descriptor_tables.size()+root_constants.size(); i++) {
		for (const DescriptorTable &table : descriptor_tables) {
			if (table.parameter_index == i)
				compilation_params.push_back(table.root_parameters[0]);
		}
		for (const RootConstants* constants : root_constants) {
			if (constants->parameter_index == i)
				compilation_params.push_back(constants->root_parameters[0]);
		}
	}

	// create a static sampler
	D3D12_STATIC_SAMPLER_DESC sampler = { };
	sampler.Filter = D3D12_FILTER_MINIMUM_ANISOTROPIC;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = 0.0f;
	sampler.MaxAnisotropy = 0u;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_NEVER;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_TRANSPARENT_BLACK;
	sampler.MinLOD = 0.0f;
	sampler.MaxLOD = D3D12_FLOAT32_MAX;
	sampler.ShaderRegister = 0u;
	sampler.RegisterSpace = 0u;
	sampler.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL;

	signature_desc.Init((UINT)compilation_params.size(), (compilation_params.empty() ? nullptr : &compilation_params[0]), 1u, &sampler,
		D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT | // we can deny shader stages here for better performance
		D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS |
		D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS);

	ComPtr<ID3DBlob> signature_blob;
	ComPtr<ID3DBlob> error_buf;
	HPEW(D3D12SerializeRootSignature(&signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, &signature_blob, &error_buf), HPEW_ERR_BLOB_PARAM(error_buf));

	HPEW(device->CreateRootSignature(0, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&signature)));
	HPEW(signature->SetName(L"Main Root Signature"));

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
			HPEW(descriptor_heaps[i]->SetName(string_to_wstring("CBV/SRV/UAV Descriptor Heap #" + std::to_string(i)).c_str()));
		}
	}

	for (ShaderResourceView *srv : shader_resource_views) {
		srv->compile(device, command_list, descriptor_heaps);
	}
	for (ConstantBuffer* cb : constant_buffers) {
		cb->compile(device, descriptor_heaps);
	}
}

void GraphicsPipeline::RootSignature::update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, int frame_index) {
	for (ShaderResourceView* srv : shader_resource_views) {
		if (srv->compile_signal) {
			srv->compile(device, command_list, descriptor_heaps);
		}
	}

	for (ConstantBuffer* cb : constant_buffers) {
		if (cb->update_signal) {
			for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
				cb->apply(i);
			}
		}
	}

	// set constant buffer descriptor heap
	ID3D12DescriptorHeap* heaps[] = { descriptor_heaps[frame_index].Get() };
	if (!descriptor_tables.empty()) { // descriptor_heaps is only empty when descriptor_tables is as well
		command_list->SetDescriptorHeaps(_countof(heaps), heaps);
	}

	for (const DescriptorTable &descriptor_table : descriptor_tables) {
		D3D12_GPU_DESCRIPTOR_HANDLE handle =
			descriptor_heaps[frame_index]->GetGPUDescriptorHandleForHeapStart();
		handle.ptr += descriptor_table.heap_index *
			device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		command_list->SetGraphicsRootDescriptorTable(descriptor_table.parameter_index, handle);
	}

	for (const RootConstants* constants : root_constants) {
		command_list->SetGraphicsRoot32BitConstants(constants->parameter_index, constants->constants.Num32BitValues, constants->obj, 0u);
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
	UINT parameter_index = index + (UINT)shader_resource_views.size();
	descriptor_tables.push_back(DescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, shader, index, (UINT)descriptor_tables.size(), parameter_index));
	cb.heap_index = descriptor_tables.back().heap_index;
	constant_buffers.push_back(&cb);
}

void GraphicsPipeline::RootSignature::bind_shader_resource_view(ShaderResourceView &srv, D3D12_SHADER_VISIBILITY shader) {
	UINT index = (UINT)shader_resource_views.size();
	UINT parameter_index = index + (UINT)constant_buffers.size() + (UINT)root_constants.size();
	descriptor_tables.push_back(DescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, shader, index, (UINT)descriptor_tables.size(), parameter_index));
	srv.index = (UINT)shader_resource_views.size();
	srv.heap_index = descriptor_tables.back().heap_index;
	shader_resource_views.push_back(&srv);
}

GraphicsPipeline::RootSignature::RootArgument::RootArgument(UINT parameter_index)
	: root_parameters(std::vector<D3D12_ROOT_PARAMETER>(1u)),
	parameter_index(parameter_index) { }

GraphicsPipeline::RootSignature::DescriptorTable::DescriptorTable(D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_SHADER_VISIBILITY shader, UINT index, UINT heap_index, UINT parameter_index)
	: RootArgument(parameter_index), ranges(std::vector<D3D12_DESCRIPTOR_RANGE>(1u)),
	heap_index(heap_index) {
	compile(type, shader, index);
}

void GraphicsPipeline::RootSignature::DescriptorTable::compile(D3D12_DESCRIPTOR_RANGE_TYPE type, D3D12_SHADER_VISIBILITY shader, UINT index) {
	ranges[0].RangeType = type;
	ranges[0].NumDescriptors = 1;
	ranges[0].BaseShaderRegister = index;
	ranges[0].RegisterSpace = 0; // space 0. can usually be zero
	ranges[0].OffsetInDescriptorsFromTableStart = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND; // this appends the range to the end of the root signature descriptor tables
	
	table.NumDescriptorRanges = (UINT)ranges.size();
	table.pDescriptorRanges = &ranges[0];
	
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
	update_signal = false;
}

bool GraphicsPipeline::RootSignature::ConstantBuffer::operator==(const ConstantBuffer &cb) const noexcept {
	for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
		if (gpu_addresses[i] != cb.gpu_addresses[i]) return false;
	}
	
	return (//obj == cb.obj && std::equal_to<void> no matching call operator found
		obj_size == cb.obj_size &&
		name == cb.name);
}

GraphicsPipeline::RootSignature::ShaderResourceView::ShaderResourceView(const DirectX::ScratchImage &mip_chain, bool is_texture_cube) {
	update_descs(mip_chain, is_texture_cube);
}

void GraphicsPipeline::RootSignature::ShaderResourceView::update_descs(const DirectX::ScratchImage &mip_chain, bool is_texture_cube) {
	const Image &chain_base = *mip_chain.GetImages();
	heap_desc = D3D12_RESOURCE_DESC {
		.Dimension = D3D12_RESOURCE_DIMENSION_TEXTURE2D,
		.Width = (UINT)chain_base.width,
		.Height = (UINT)chain_base.height,
		.DepthOrArraySize = 1,
		.MipLevels = (UINT16)mip_chain.GetImageCount(),
		.Format = chain_base.format,
		.SampleDesc = {.Count = 1 },
		.Layout = D3D12_TEXTURE_LAYOUT_UNKNOWN,
		.Flags = D3D12_RESOURCE_FLAG_NONE,
	};

	// collect subresource data 
	subresources = std::ranges::views::iota(0, (int)mip_chain.GetImageCount()) |
		std::ranges::views::transform([&](int i) {
		const auto img = mip_chain.GetImage(i, 0, 0);
		return D3D12_SUBRESOURCE_DATA {
			.pData = img->pixels,
			.RowPitch = (LONG_PTR)img->rowPitch,
			.SlicePitch = (LONG_PTR)img->slicePitch,
		};
	}) |
		std::ranges::to<std::vector>();

	srv_desc = D3D12_SHADER_RESOURCE_VIEW_DESC {
		.Format = heap_desc.Format,
		.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D,
		.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING,
		.Texture2D{.MipLevels = heap_desc.MipLevels },
	};

	if (is_texture_cube) {
		srv_desc.TextureCube.MipLevels = (UINT)mip_chain.GetImageCount();
	} else {
		srv_desc.Texture2D.MipLevels = (UINT)mip_chain.GetImageCount();
	}
}

void GraphicsPipeline::RootSignature::ShaderResourceView::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const ComPtr<ID3D12DescriptorHeap> descriptor_heaps[NUMBER_OF_BUFFERS]) {	
	if (heap_desc.Width != 0 && heap_desc.Height != 0) {
		ResourceManager::Release::resources.push_back(default_buffer);

		auto props = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
		HPEW(device->CreateCommittedResource(
			&props,
			D3D12_HEAP_FLAG_NONE,
			&heap_desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(&default_buffer)
		));
		HPEW(default_buffer->SetName(string_to_wstring("SRV Default Heap of index " + std::to_string(index)).c_str()));

		// create the intermediate upload buffer 
		ID3D12Resource* upload_buffer;
		const CD3DX12_HEAP_PROPERTIES heapProps{ D3D12_HEAP_TYPE_UPLOAD };
		const auto uploadBufferSize = GetRequiredIntermediateSize(
			default_buffer.Get(), 0, (UINT)subresources.size()
		);
		const auto resourceDesc = CD3DX12_RESOURCE_DESC::Buffer(uploadBufferSize);
		HPEW(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&resourceDesc,
			D3D12_RESOURCE_STATE_GENERIC_READ,
			nullptr,
			IID_PPV_ARGS(&upload_buffer)
		));

		ResourceManager::Release::resources.push_back(upload_buffer);
		HPEW(upload_buffer->SetName(L"Shader resource view upload buffer"));

		// write commands to copy data to upload texture (copying each subresource) 
		UpdateSubresources(
			command_list.Get(),
			default_buffer.Get(),
			upload_buffer,
			0, 0,
			(UINT)subresources.size(),
			subresources.data()
		);

		compile_signal = false;

		// write command to transition texture to texture state  
		/*{
		const auto barrier = CD3DX12_RESOURCE_BARRIER::Transition(
		default_heap.Get(),
		D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
		command_list->ResourceBarrier(1, &barrier);
		}*/

		for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
			// Taking the first descriptor of the heap as a simple example  
			D3D12_CPU_DESCRIPTOR_HANDLE handle = { };
			handle.ptr = descriptor_heaps[i]->GetCPUDescriptorHandleForHeapStart().ptr + heap_index * device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV); 
			device->CreateShaderResourceView(default_buffer.Get(), &srv_desc, handle);
		}
	}
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
