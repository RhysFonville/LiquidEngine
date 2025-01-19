#include "../GraphicsPipeline.h"

void GraphicsPipeline::RootSignature::compile(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heaps, const Shaders& shaders) {
	bind_shader_resource_parameters(shaders.vs);
	bind_shader_resource_parameters(shaders.hs);
	bind_shader_resource_parameters(shaders.ds);
	bind_shader_resource_parameters(shaders.gs);
	bind_shader_resource_parameters(shaders.ps);

	D3D12_STATIC_SAMPLER_DESC sampler = {};
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
	
	auto params{get_root_params()};
	
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC signature_desc = {
		.Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
		.Desc_1_1 = {
			.NumParameters = (UINT)params.size(),
			.pParameters = (
				params.empty() ? nullptr
				: params.data()
			),
			.NumStaticSamplers = 1u,
			.pStaticSamplers = &sampler,
			// Can deny shader stages here for better performance
			.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		}
	};

	ComPtr<ID3DBlob> signature_blob;
	ComPtr<ID3DBlob> error_buf;
	HPEW(D3D12SerializeVersionedRootSignature(&signature_desc, signature_blob.GetAddressOf(), error_buf.GetAddressOf()));

	HPEW(device->CreateRootSignature(0u, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&signature)));
	HPEW(signature->SetName(L"Root Signature"));

	signature_blob.Reset();
	error_buf.Reset();
}

void GraphicsPipeline::RootSignature::compile_resources(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heaps) {
	for (const std::pair<std::string, std::shared_ptr<ConstantBuffer>>& cb : constant_buffers) {
		cb.second->compile(device, command_list, descriptor_heaps);
	}
	for (const std::pair<std::string, std::shared_ptr<ShaderResourceView>>& srv : shader_resource_views) {
		srv.second->compile(device, command_list, descriptor_heaps);
	}
}

void GraphicsPipeline::RootSignature::bind_shader_resource_parameters(const std::weak_ptr<Shader>& s) {
	std::shared_ptr<Shader> shader{nullptr};
	if (std::shared_ptr<Shader> shared{s.lock()}) shader = shared;
	else return;

	ComPtr<ID3D12ShaderReflection> reflection{shader->get_reflection()};

	D3D12_SHADER_DESC desc{};
	HPEW(reflection->GetDesc(&desc));

	for (const uint32_t i : std::views::iota(0u, desc.BoundResources)) {
		D3D12_SHADER_INPUT_BIND_DESC shader_input_bind_desc{};
		HPEW(reflection->GetResourceBindingDesc(i, &shader_input_bind_desc));

		if (shader_input_bind_desc.Type == D3D_SIT_CBUFFER) {
			//root_param_index_map[shader_input_bind_desc.Name] = (UINT)root_params.size();
			
			std::shared_ptr<ConstantBuffer> cb{std::make_shared<ConstantBuffer>(
				ConstantBuffer{
					DescriptorTable{
						shader_input_bind_desc,
						D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
						(UINT)descriptor_tables.size()
					}
				}
			)};

			constant_buffers.insert(
				std::make_pair(std::string{shader_input_bind_desc.Name}, cb)
			);
			descriptor_tables.push_back(&cb->descriptor_table);
		} else if (shader_input_bind_desc.Type == D3D_SIT_TEXTURE) {
			// For now, each individual texture belongs in its own descriptor table. This can cause the root signature to quickly exceed the 64WORD size limit.
			//root_param_index_map[shader_input_bind_desc.Name] = (UINT)root_params.size();
			
			std::shared_ptr<ShaderResourceView> srv{std::make_shared<ShaderResourceView>(
				ShaderResourceView{
					DescriptorTable{
						shader_input_bind_desc,
						D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
						(UINT)descriptor_tables.size()
					}
				}
			)};
			
			shader_resource_views.insert(
				std::make_pair(std::string{shader_input_bind_desc.Name}, srv)
			);
			descriptor_tables.push_back(&srv->descriptor_table);
		}
	}
}

void GraphicsPipeline::RootSignature::clean_up() {
	auto rm = []<typename T, typename U>(std::map<T, U>& resources) {
		for (auto& resource : resources) {
			resource.second->clean_up();
		}
		resources.clear();
	};

	rm(constant_buffers);
	rm(shader_resource_views);
	//rm(root_constants);

	descriptor_tables.clear();
	signature.Reset();
}

void GraphicsPipeline::RootSignature::check_for_update(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heaps) {
	int i{0};
	for (const std::pair<std::string, std::shared_ptr<ConstantBuffer>>& cb : constant_buffers) {
		if (cb.second->will_compile()) {
			cb.second->compile(device, command_list, descriptor_heaps);
		}
		i++;
	}

	i = 0;
	for (const std::pair<std::string, std::shared_ptr<ConstantBuffer>>& cb : constant_buffers) {
		if (cb.second->will_update()) {
			cb.second->update(device, command_list);
		}
		i++;
	}

	i = 0;
	for (const std::pair<std::string, std::shared_ptr<ShaderResourceView>>& srv : shader_resource_views) {
		if (srv.second->will_compile()) {
			srv.second->compile(device, command_list, descriptor_heaps);
		}
		i++;
	}
}

void GraphicsPipeline::RootSignature::run(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heap) {
	command_list->SetGraphicsRootSignature(signature.Get()); // set the root signature

	for (const DescriptorTable* dt : descriptor_tables) {
		dt->set_descriptor_table(device, command_list, descriptor_heap);
	}

	/*for (const auto& constants : root_constants) {
		constants->set_constants(command_list);
	}*/
}

bool GraphicsPipeline::RootSignature::operator==(const RootSignature& root_signature) const noexcept {
	return (signature == root_signature.signature);
}

/*void GraphicsPipeline::RootSignature::bind_constant_buffer(std::shared_ptr<ConstantBuffer>& cb, D3D12_SHADER_VISIBILITY shader) {
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
}*/

void GraphicsPipeline::RootSignature::create_views(const ComPtr<ID3D12Device>& device, GraphicsResourceDescriptorHeap& descriptor_heaps) {
	for (const auto& cb : constant_buffers) {
		cb.second->create_view(device, descriptor_heaps);
	}
	for (const auto& srv : shader_resource_views) {
		srv.second->create_view(device, descriptor_heaps);
	}
}

std::vector<DescriptorTable> GraphicsPipeline::RootSignature::get_descriptor_tables() const noexcept {
	std::vector<DescriptorTable> dts{};
	dts.reserve(descriptor_tables.size());
	for (const DescriptorTable* dt : descriptor_tables) {
		dts.push_back(*dt);
	}

	return dts;
}

std::vector<D3D12_ROOT_PARAMETER1> GraphicsPipeline::RootSignature::get_root_params() const noexcept {
	std::vector<D3D12_ROOT_PARAMETER1> params{};
	params.reserve(descriptor_tables.size());
	for (const DescriptorTable* dt : descriptor_tables) {
		params.push_back(dt->get_root_param());
	}

	return params;
}

std::weak_ptr<DescriptorRootObject> GraphicsPipeline::RootSignature::get_resource(const std::string& name) const {
	auto cb{constant_buffers.find(name)};
	auto srv{shader_resource_views.find(name)};

	if (cb != constant_buffers.end())
		return std::static_pointer_cast<DescriptorRootObject>(cb->second);
	else if (srv != shader_resource_views.end())
		return std::static_pointer_cast<DescriptorRootObject>(srv->second);
	else
		throw std::exception{"Can't find root signature resource of that name."};
}

std::weak_ptr<ConstantBuffer> GraphicsPipeline::RootSignature::get_constant_buffer(const std::string& name) const {
	if (auto cb{constant_buffers.find(name)}; cb != constant_buffers.end()) {
		return cb->second;
	} else {
		return std::weak_ptr<ConstantBuffer>{};
	}
}

std::weak_ptr<ShaderResourceView> GraphicsPipeline::RootSignature::get_shader_resource_view(const std::string& name) const {
	if (auto srv{shader_resource_views.find(name)}; srv != shader_resource_views.end()) {
		return srv->second;
	} else {
		return std::weak_ptr<ShaderResourceView>{};
	}
}

