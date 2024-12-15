#include "GraphicsPipeline.h"

void GraphicsPipeline::check_for_update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_BLEND_DESC &blend_desc, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	if (compile_signal) {
		compile(device, command_list, sample_desc, blend_desc, descriptor_heaps);
		compile_signal = false;
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
	root_signature.check_for_update(device, command_list, descriptor_heaps);
	root_signature.compile(device, command_list, descriptor_heaps);

	// Fill PSO

	// fill out an input layout desc structure
	D3D12_INPUT_LAYOUT_DESC input_layout_desc = {};

	input_layout_desc.NumElements = (UINT)input_layout.size();
	input_layout_desc.pInputElementDescs = &input_layout[0];

	D3D12_GRAPHICS_PIPELINE_STATE_DESC pso_desc = {}; // a structure to define a pso
	pso_desc.pRootSignature = root_signature.signature.Get(); // the root signature that describes the input data this pso needs
	pso_desc.InputLayout = input_layout_desc; // the structure describing our input layout
	pso_desc.PrimitiveTopologyType = input_assembler.primitive_topology_type; // type of topology we are drawing
	if (!this->vs.expired()) pso_desc.VS = this->vs.lock()->get_bytecode(); // structure describing where to find the vertex shader bytecode and how large it is
	if (!this->hs.expired()) pso_desc.HS = this->hs.lock()->get_bytecode();
	if (!this->ds.expired()) pso_desc.DS = this->ds.lock()->get_bytecode();
	pso_desc.StreamOutput = stream_output.desc;
	if (!this->gs.expired()) pso_desc.GS = this->gs.lock()->get_bytecode();
	pso_desc.RasterizerState = rasterizer.desc;
	if (!this->ps.expired()) pso_desc.PS = this->ps.lock()->get_bytecode();
	pso_desc.DepthStencilState = depth_stencil_desc; // a default depth stencil state
	pso_desc.RTVFormats[0] = DXGI_FORMAT_R8G8B8A8_UNORM; // format of the render target
	pso_desc.SampleDesc = sample_desc; // must be the same sample desc as the swapchain and depth/stencil buffer
	pso_desc.SampleMask = UINT_MAX; // sample mask has to do with multi-sampling. 0xffffffff means point sampling is done
	pso_desc.BlendState = blend_desc; // a default blend state.
	pso_desc.NumRenderTargets = 1; // we are only binding one render target
	pso_desc.DSVFormat = DXGI_FORMAT_D32_FLOAT;
	//pso_desc.Flags = D3D12_PIPELINE_STATE_FLAG_TOOL_DEBUG;

	// create the pso
	HPEW(device->CreateGraphicsPipelineState(&pso_desc, IID_PPV_ARGS(&pipeline_state_object)));
	HPEW(pipeline_state_object->SetName(L"Main PSO"));
}

void GraphicsPipeline::clean_up() {
	pipeline_state_object.Reset();

	input_assembler.clean_up();
	root_signature.clean_up();
}

bool GraphicsPipeline::operator==(const GraphicsPipeline &pipeline) const noexcept {
	return (pipeline_state_object == pipeline.pipeline_state_object &&
	root_signature == pipeline.root_signature &&
	input_assembler == pipeline.input_assembler &&

	vs.lock() == pipeline.vs.lock() &&
	hs.lock() == pipeline.hs.lock() &&
	ds.lock() == pipeline.ds.lock() &&
	gs.lock() == pipeline.gs.lock() &&
	ps.lock() == pipeline.ps.lock() &&

	rasterizer == pipeline.rasterizer &&
	stream_output == pipeline.stream_output);
}

// +-----------------+
// | Input Assembler |
// +-----------------+

void GraphicsPipeline::InputAssembler::set_instances(const std::vector<Transform>& instances, const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList>& command_list) {
	ComPtr<ID3D12Resource> instance_buffer_upload{nullptr};
	auto upload_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto upload_buffer = CD3DX12_RESOURCE_DESC::Buffer(instances.size() * sizeof(Transform));
	device->CreateCommittedResource(
		&upload_heap_properties, // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&upload_buffer, // resource desc for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&instance_buffer_upload));

	resource_manager->add_resource_to_release(instance_buffer_upload);
	HPEW(instance_buffer_upload->SetName(L"Instance Buffer Upload Resource Heap"));

	void* p = nullptr;
	instance_buffer_upload->Map(0, nullptr, &p);
	memcpy(p, &instances[0], sizeof(Transform) * instances.size());
	instance_buffer_upload->Unmap(0, nullptr);

	auto default_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	auto default_buffer = CD3DX12_RESOURCE_DESC::Buffer(instances.size() * sizeof(Transform));
	device->CreateCommittedResource(
		&default_heap_properties, // a default heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&default_buffer, // resource desc for a buffer
		D3D12_RESOURCE_STATE_COPY_DEST, // we will start this heap in the copy destination state since we will copy data
		// from the upload heap to this heap
		nullptr, // optimized clear value must be null for this type of resource. used for render targets and depth/stencil buffers
		IID_PPV_ARGS(&instance_buffer)
	);

	HPEW(instance_buffer->SetName(L"Instance Buffer Default Resource Heap"));

	command_list->CopyResource(instance_buffer.Get(), instance_buffer_upload.Get());

	instance_buffer_view.BufferLocation = instance_buffer->GetGPUVirtualAddress();
	instance_buffer_view.SizeInBytes = (UINT)instances.size() * sizeof(Transform);
	instance_buffer_view.StrideInBytes = sizeof(Transform);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	auto transition = CD3DX12_RESOURCE_BARRIER::Transition(instance_buffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	command_list->ResourceBarrier(1, &transition);
}

void GraphicsPipeline::InputAssembler::add_mesh(const Mesh &mesh, const ComPtr<ID3D12Device> &device,
	const ComPtr<ID3D12GraphicsCommandList> &command_list, size_t index) {

	const std::vector<Vertex> &verts = mesh.get_vertices();

 	if (index == (size_t)-1) {
		index = vertex_buffers.size();
	}

	ComPtr<ID3D12Resource> vertex_buffer_upload{nullptr};
	auto upload_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto upload_buffer = CD3DX12_RESOURCE_DESC::Buffer(verts.size() * sizeof(Vertex));
	device->CreateCommittedResource(
		&upload_heap_properties, // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&upload_buffer, // resource desc for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&vertex_buffer_upload));

	resource_manager->add_resource_to_release(vertex_buffer_upload);
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

	command_list->CopyResource(vertex_buffers[index].Get(), vertex_buffer_upload.Get());
	
	vertex_buffer_views[index].BufferLocation = vertex_buffers[index]->GetGPUVirtualAddress();
	vertex_buffer_views[index].SizeInBytes = (UINT)verts.size() * sizeof(Vertex);
	vertex_buffer_views[index].StrideInBytes = sizeof(Vertex);

	// transition the vertex buffer data from copy destination state to vertex buffer state
	auto transition = CD3DX12_RESOURCE_BARRIER::Transition(vertex_buffers[index].Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	command_list->ResourceBarrier(1, &transition);
}

void GraphicsPipeline::InputAssembler::remove_mesh(size_t index) {
	resource_manager->add_resource_to_release(vertex_buffers[index]);
	vertex_buffers.erase(vertex_buffers.begin() + index);
	vertex_buffer_views.erase(vertex_buffer_views.begin() + index);
}

void GraphicsPipeline::InputAssembler::remove_all_meshes() {
	for (auto &buffer : vertex_buffers) {
		resource_manager->add_resource_to_release(buffer);
	}

	vertex_buffers.clear();
	vertex_buffer_views.clear();
}

void GraphicsPipeline::InputAssembler::check_for_update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list) {
	while (!commands.empty()) {
		auto command{commands.front()};

		if (command->get_type() == GraphicsPipelineIACommand::Type::AddMesh) {
			auto add{std::static_pointer_cast<GraphicsPipelineIAAddMeshCommand>(command)};
			auto index{add->get_index()};
			add_mesh(*add->get_mesh(), device, command_list, (index.has_value() ? index.value() : (size_t)-1));
		} else if (command->get_type() == GraphicsPipelineIACommand::Type::RemoveMesh) {
			auto remove{std::static_pointer_cast<GraphicsPipelineIARemoveMeshCommand>(command)};
			remove_mesh(remove->get_index());
		} else if (command->get_type() == GraphicsPipelineIACommand::Type::RemoveAll) {
			remove_all_meshes();
		} else {
			auto set{std::static_pointer_cast<GraphicsPipelineIASetInstancesCommand>(command)};
			set_instances(set->get_instances(), device, command_list);
		}

		commands.pop();
	}
}

void GraphicsPipeline::InputAssembler::run(const ComPtr<ID3D12GraphicsCommandList> &command_list) {
	command_list->IASetPrimitiveTopology(primitive_topology); // set the primitive topology
	command_list->IASetVertexBuffers(0, (UINT)vertex_buffer_views.size(), vertex_buffer_views.data()); // set the vertex buffer (using the vertex buffer view)
	command_list->IASetVertexBuffers(1u, 1u, &instance_buffer_view);
}

void GraphicsPipeline::InputAssembler::draw_meshes(const ComPtr<ID3D12GraphicsCommandList> &command_list) {
	UINT verts = 0;
	for (const D3D12_VERTEX_BUFFER_VIEW &view : vertex_buffer_views) {
		verts += view.SizeInBytes / sizeof(Vertex);
	}

	command_list->DrawInstanced(
		verts,
		instance_buffer_view.SizeInBytes / sizeof(Transform),
		0u, 0u
	);
}

void GraphicsPipeline::InputAssembler::clean_up() {
	/*for (auto &vb : vertex_buffers) {
		vb.Reset();
	}
	vertex_buffers.clear();
	vertex_buffer_views.clear();*/
	remove_all_meshes();
}

const std::vector<D3D12_VERTEX_BUFFER_VIEW> & GraphicsPipeline::InputAssembler::get_vertex_buffer_views() const noexcept {
	return vertex_buffer_views;
}

D3D12_VERTEX_BUFFER_VIEW GraphicsPipeline::InputAssembler::get_instance_buffer_view() const noexcept {
	return instance_buffer_view;
}

// +----------------+
// | Root Signature |
// +----------------+

void GraphicsPipeline::RootSignature::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	/*for (ShaderResourceView* srv : shader_resource_views) {
		srv->compile(device, command_list, descriptor_heaps);
	}
	for (ConstantBuffer* cb : constant_buffers) {
		cb->compile(device, command_list, descriptor_heaps);
	}*/
	
	compilation_params.clear();
	for (int i = 0; i < descriptor_tables.size()+root_constants.size(); i++) {
		for (const auto& dt_wp : descriptor_tables) {
			if (const auto& dt = dt_wp.lock())
				if (dt->get_parameter_index() == i)
					compilation_params.push_back(dt->get_root_parameters()[0]);
		}
		for (const auto& constants_wp : root_constants) {
			if (const auto& constants = constants_wp.lock())
				if (constants->get_parameter_index() == i)
					compilation_params.push_back(constants->get_root_parameters()[0]);
		}
	}

	// create a static sampler
	D3D12_STATIC_SAMPLER_DESC sampler = { };
	sampler.Filter = D3D12_FILTER_ANISOTROPIC;
	sampler.AddressU = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressV = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.AddressW = D3D12_TEXTURE_ADDRESS_MODE_WRAP;
	sampler.MipLODBias = 0.0f;
	sampler.MaxAnisotropy = 16u;
	sampler.ComparisonFunc = D3D12_COMPARISON_FUNC_LESS_EQUAL;
	sampler.BorderColor = D3D12_STATIC_BORDER_COLOR_OPAQUE_WHITE;
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
	HPEW(D3D12SerializeRootSignature(&signature_desc, D3D_ROOT_SIGNATURE_VERSION_1, signature_blob.GetAddressOf(), error_buf.GetAddressOf()), HPEW_ERR_BLOB_PARAM(error_buf));

	HPEW(device->CreateRootSignature(0u, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&signature)));
	HPEW(signature->SetName(L"Main Root Signature"));

	signature_blob.Reset();
	error_buf.Reset();
}

void GraphicsPipeline::RootSignature::check_for_update(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	int i{0};
	for (const std::weak_ptr<ShaderResourceView>& srv_wp : shader_resource_views) {
		if (const auto& srv = srv_wp.lock()) {
			if (srv->compile_signal) {
				srv->compile(device, command_list, descriptor_heaps);
			}
		} else {
			shader_resource_views.erase(shader_resource_views.begin()+i);
		}
		i++;
	}

	i = 0;
	for (const std::weak_ptr<ConstantBuffer>& cb_wp : constant_buffers) {
		if (const auto& cb = cb_wp.lock()) {
			if (cb->compile_signal) {
				cb->compile(device, command_list, descriptor_heaps);
			}
		} else {
			constant_buffers.erase(constant_buffers.begin()+i);
		}
		i++;
	}

	i = 0;
	for (const std::weak_ptr<ConstantBuffer>& cb_wp : constant_buffers) {
		if (const auto& cb = cb_wp.lock()) {
			if (cb->update_signal) {
				for (int i = 0; i < NUMBER_OF_BUFFERS; i++) {
					cb->update(device, command_list);
				}
			}
		} else {
			constant_buffers.erase(constant_buffers.begin()+i);
		}
		i++;
	}
}

void GraphicsPipeline::RootSignature::clean_up() {
	auto rm = []<typename T>(std::vector<std::weak_ptr<T>>&vec) {
		for (const std::weak_ptr<T>& wp : vec) {
			if (const std::shared_ptr<T>& cb = wp.lock()) {
				cb->clean_up();
			}
		}
		vec.clear();
	};

	rm(constant_buffers);
	rm(shader_resource_views);
	rm(root_constants);
	rm(descriptor_tables);

	compilation_params.clear();
	signature.Reset();
}

void GraphicsPipeline::RootSignature::run(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, GraphicsResourceDescriptorHeap &descriptor_heap) {
	command_list->SetGraphicsRootSignature(signature.Get()); // set the root signature

	for (const std::weak_ptr<DescriptorTable>& dt_wp : descriptor_tables) {
		if (const auto& dt = dt_wp.lock())
			dt->set_descriptor_table(device, command_list, descriptor_heap);
	}

	for (const auto& constants_wp : root_constants) {
		if (const auto& constants = constants_wp.lock()) constants->set_constants(command_list);
	}
}

bool GraphicsPipeline::RootSignature::operator==(const RootSignature &root_signature) const noexcept {
	return (signature == root_signature.signature &&
		signature_desc == root_signature.signature_desc);
}

void GraphicsPipeline::RootSignature::bind_constant_buffer(std::shared_ptr<ConstantBuffer>& cb, D3D12_SHADER_VISIBILITY shader) {
	UINT index = (UINT)constant_buffers.size() + (UINT)root_constants.size();
	UINT parameter_index = index + (UINT)shader_resource_views.size();

	cb->descriptor_table = std::make_shared<DescriptorTable>(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, shader, index, parameter_index);
	cb->compile();

	descriptor_tables.push_back(cb->descriptor_table);
	constant_buffers.push_back(cb);
}

void GraphicsPipeline::RootSignature::bind_shader_resource_view(std::shared_ptr<ShaderResourceView>& srv, D3D12_SHADER_VISIBILITY shader) {
	UINT index = (UINT)shader_resource_views.size();
	UINT parameter_index = index + (UINT)constant_buffers.size() + (UINT)root_constants.size();
	
	srv->descriptor_table = std::make_shared<DescriptorTable>(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, shader, index, parameter_index);
	srv->compile();

	descriptor_tables.push_back(srv->descriptor_table);
	shader_resource_views.push_back(srv);
}

void GraphicsPipeline::RootSignature::create_views(const ComPtr<ID3D12Device> &device, GraphicsResourceDescriptorHeap &descriptor_heaps) {
	for (const auto& cb_wp : constant_buffers) {
		if (const auto& cb = cb_wp.lock()) {
			if (cb->valid()) {
				cb->create_views(device, descriptor_heaps);
			}
		}
	}
	for (const auto& srv_wp : shader_resource_views) {
		if (const auto& srv = srv_wp.lock()) {
			if (srv->valid()) {
				srv->create_views(device, descriptor_heaps);
			}
		}
	}
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
