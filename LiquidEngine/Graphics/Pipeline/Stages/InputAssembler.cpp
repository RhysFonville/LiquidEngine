#include "../GraphicsPipeline.h"

bool GraphicsPipeline::InputAssembler::operator==(const InputAssembler& input_assembler) const noexcept {
	return (
		vertex_buffers == input_assembler.vertex_buffers &&
		vertex_buffer_views == input_assembler.vertex_buffer_views &&
		primitive_topology_type == input_assembler.primitive_topology_type &&
		primitive_topology == input_assembler.primitive_topology
	);
}

static DXGI_FORMAT mask_to_format(int mask) {
	switch (mask) {
	case 1:  // b'001
		return DXGI_FORMAT_R32_FLOAT;          // float
	case 3:  // b'011
		return DXGI_FORMAT_R32G32_FLOAT;       // float2
	case 7:  // b'111
		return DXGI_FORMAT_R32G32B32_FLOAT;    // float3
	case 15: // b'1111
		return DXGI_FORMAT_R32G32B32A32_FLOAT; // float4
	default:
		return DXGI_FORMAT_UNKNOWN;
	}
}


void GraphicsPipeline::InputAssembler::compile(const std::weak_ptr<Shader>& vs) {
	std::shared_ptr<Shader> shader{nullptr};
	if (std::shared_ptr<Shader> sp{vs.lock()}) shader = sp;
	else throw std::exception{"VS weak pointer not assigned to shader pointer."};

	ComPtr<ID3D12ShaderReflection> reflection{shader->get_reflection()};
	
	D3D12_SHADER_DESC desc{};
	HPEW(reflection->GetDesc(&desc));
	input_layout.reserve(desc.InputParameters);

	input_layout.clear();
	for (const uint32_t param_index : std::views::iota(0u, desc.InputParameters)) {
		D3D12_SIGNATURE_PARAMETER_DESC signature_param_desc{};
		reflection->GetInputParameterDesc(param_index, &signature_param_desc);

		input_layout.emplace_back(D3D12_INPUT_ELEMENT_DESC{
			.SemanticName = signature_param_desc.SemanticName,
			.SemanticIndex = signature_param_desc.SemanticIndex,
			.Format = mask_to_format(signature_param_desc.Mask),
			.InputSlot = 0u,
			.AlignedByteOffset = D3D12_APPEND_ALIGNED_ELEMENT,
			.InputSlotClass = D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,
			// Does not support automating instancing??? :(
			.InstanceDataStepRate = 0u,
		});
	}
}

/*void GraphicsPipeline::InputAssembler::set_instances(const std::vector<Transform>& instances, const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list) {
	ComPtr<ID3D12Resource> instance_buffer_upload{nullptr};
	auto upload_heap_properties = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	auto upload_buffer = CD3DX12_RESOURCE_DESC::Buffer(instances.size() * sizeof(Transform));
	device->CreateCommittedResource(
		&upload_heap_properties, // upload heap
		D3D12_HEAP_FLAG_NONE, // no flags
		&upload_buffer, // resource desc for a buffer
		D3D12_RESOURCE_STATE_GENERIC_READ, // GPU will read from this buffer and copy its contents to the default heap
		nullptr,
		IID_PPV_ARGS(&instance_buffer_upload)
	);

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
}*/

void GraphicsPipeline::InputAssembler::add_mesh(const Mesh& mesh, const ComPtr<ID3D12Device>& device,
	const ComPtr<ID3D12GraphicsCommandList>& command_list, size_t index) {

	const std::vector<Vertex>& verts = mesh.get_vertices();

	if (verts.size() == 0) {
		vertex_buffer_views.push_back(D3D12_VERTEX_BUFFER_VIEW{});
		vertex_buffers.push_back(nullptr);
		return;
	}

	if (index == (size_t)-1) {
		index = vertex_buffer_views.size();
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
	for (auto& buffer : vertex_buffers) {
		resource_manager->add_resource_to_release(buffer);
	}

	vertex_buffers.clear();
	vertex_buffer_views.clear();
}

void GraphicsPipeline::InputAssembler::check_for_update(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list) {
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
			//set_instances(set->get_instances(), device, command_list);
		}

		commands.pop();
	}
}

void GraphicsPipeline::InputAssembler::run(const ComPtr<ID3D12GraphicsCommandList>& command_list) {
	if (vertex_buffer_views[buffer_view_to_draw].SizeInBytes == 0u) return;
	
	command_list->IASetPrimitiveTopology(primitive_topology);
	command_list->IASetVertexBuffers(0u, 1u, &vertex_buffer_views[buffer_view_to_draw]);
	//command_list->IASetVertexBuffers(1u, 1u, &instance_buffer_view);
}

void GraphicsPipeline::InputAssembler::draw_meshes(const ComPtr<ID3D12GraphicsCommandList>& command_list) {
	if (vertex_buffer_views[buffer_view_to_draw].SizeInBytes == 0u) return;
	
	command_list->DrawInstanced(
		vertex_buffer_views[buffer_view_to_draw].SizeInBytes / sizeof(Vertex),
		/*instance_buffer_view.SizeInBytes / sizeof(Transform)*/1u,
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

const std::vector<D3D12_VERTEX_BUFFER_VIEW>& GraphicsPipeline::InputAssembler::get_vertex_buffer_views() const noexcept {
	return vertex_buffer_views;
}

void GraphicsPipeline::InputAssembler::set_vertex_buffer_view_to_draw(size_t index) noexcept {
	buffer_view_to_draw = index;
}

/*D3D12_VERTEX_BUFFER_VIEW GraphicsPipeline::InputAssembler::get_instance_buffer_view() const noexcept {
	return instance_buffer_view;
}*/

void GraphicsPipeline::InputAssembler::clear_commands() noexcept {
	commands = std::queue<std::shared_ptr<GraphicsPipelineIACommand>>{};
}

void GraphicsPipeline::InputAssembler::add_command(const std::shared_ptr<GraphicsPipelineIACommand>&& command) noexcept {
	commands.push(command);
}

const std::vector<D3D12_INPUT_ELEMENT_DESC>& GraphicsPipeline::InputAssembler::get_input_layout() const noexcept {
	return input_layout;
}

void GraphicsPipeline::InputAssembler::set_input_layout(const std::vector<D3D12_INPUT_ELEMENT_DESC>& layout) noexcept {
	input_layout = layout;
}

D3D12_INPUT_LAYOUT_DESC GraphicsPipeline::InputAssembler::get_input_layout_desc() const noexcept {
	return D3D12_INPUT_LAYOUT_DESC{
		.pInputElementDescs = input_layout.data(),
		.NumElements = (UINT)input_layout.size()
	};
}
