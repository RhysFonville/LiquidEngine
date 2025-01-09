#include "../GraphicsPipeline.h"

void GraphicsPipeline::RootSignature::compile(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heaps, const Shaders& shaders) {
	bind_shader_resource_parameters(shaders.vs);
	bind_shader_resource_parameters(shaders.hs);
	bind_shader_resource_parameters(shaders.ds);
	bind_shader_resource_parameters(shaders.gs);
	bind_shader_resource_parameters(shaders.ps);
	
	
	/*for (ShaderResourceView* srv : shader_resource_views) {
		srv->compile(device, command_list, descriptor_heaps);
	}
	for (ConstantBuffer* cb : constant_buffers) {
		cb->compile(device, command_list, descriptor_heaps);
	}*/

	/*compilation_params.clear();
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
	}*/

	// create a static sampler
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

	signature_desc = {
		.Version = D3D_ROOT_SIGNATURE_VERSION_1_1,
		.Desc_1_1 = {
			.NumParameters = (UINT)root_params.size(),
			.pParameters = (root_params.empty() ? nullptr : root_params.data()),
			.NumStaticSamplers = 1u,
			.pStaticSamplers = &sampler,
			// Can deny shader stages here for better performance
			.Flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT
		}
	};

	ComPtr<ID3DBlob> signature_blob;
	ComPtr<ID3DBlob> error_buf;
	HPEW(D3D12SerializeVersionedRootSignature(&signature_desc, signature_blob.GetAddressOf(), error_buf.GetAddressOf()), HPEW_ERR_BLOB_PARAM(error_buf));

	HPEW(device->CreateRootSignature(0u, signature_blob->GetBufferPointer(), signature_blob->GetBufferSize(), IID_PPV_ARGS(&signature)));
	HPEW(signature->SetName(L"Root Signature"));

	signature_blob.Reset();
	error_buf.Reset();
}

void GraphicsPipeline::RootSignature::bind_shader_resource_parameters(const std::weak_ptr<Shader>& s) {
	std::shared_ptr<Shader> shader{nullptr};
	if (std::shared_ptr<Shader> shared{s.lock()}; shared) shader = shared;
	else return;

	ComPtr<ID3D12ShaderReflection> reflection{shader->get_reflection()};

	D3D12_SHADER_DESC desc{};
	HPEW(reflection->GetDesc(&desc));

	for (const uint32_t i : std::views::iota(0u, desc.BoundResources)) {
		D3D12_SHADER_INPUT_BIND_DESC shader_input_bind_desc{};
		HPEW(reflection->GetResourceBindingDesc(i, &shader_input_bind_desc));

		if (shader_input_bind_desc.Type == D3D_SIT_CBUFFER) {
			root_param_index_map[shader_input_bind_desc.Name] = (UINT)root_params.size();

			ID3D12ShaderReflectionConstantBuffer* shader_reflection_cb = reflection->GetConstantBufferByIndex(i);
			D3D12_SHADER_BUFFER_DESC cb_desc{};
			shader_reflection_cb->GetDesc(&cb_desc);

			/*D3D12_ROOT_PARAMETER1 root_param{
				.ParameterType = D3D12_ROOT_PARAMETER_TYPE_CBV,
				.DescriptorTable = {
					.ShaderRegister = shader_input_bind_desc.BindPoint,
					.RegisterSpace = shader_input_bind_desc.Space,
					.Flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE
				}
			};*/

			D3D12_DESCRIPTOR_RANGE1 range{
				D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				1u,
				shader_input_bind_desc.BindPoint,
				shader_input_bind_desc.Space,
				D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC
			};

			D3D12_ROOT_PARAMETER1 root_param{
				.DescriptorTable = {
					.NumDescriptorRanges = 1u,
					.pDescriptorRanges = &range
				}
			};

			root_params.push_back(root_param);
		} else if (shader_input_bind_desc.Type == D3D_SIT_TEXTURE) {
			// For now, each individual texture belongs in its own descriptor table. This can cause the root signature to quickly exceed the 64WORD size limit.
			root_param_index_map[shader_input_bind_desc.Name] = (UINT)root_params.size();
			
			D3D12_DESCRIPTOR_RANGE1 range{
				D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
				1u,
				shader_input_bind_desc.BindPoint,
				shader_input_bind_desc.Space,
				D3D12_DESCRIPTOR_RANGE_FLAG_DATA_STATIC
			};
			descriptor_ranges.push_back(range);

			D3D12_ROOT_PARAMETER1 root_param{
				.ParameterType = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE,
				.DescriptorTable = {
					.NumDescriptorRanges = 1u,
					.pDescriptorRanges = &descriptor_ranges.back(),
				},
				.ShaderVisibility = D3D12_SHADER_VISIBILITY_PIXEL,
			};
			root_params.push_back(root_param);
		}
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

	root_params.clear();
	signature.Reset();
}

void GraphicsPipeline::RootSignature::check_for_update(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heaps) {
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

void GraphicsPipeline::RootSignature::run(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heap) {
	command_list->SetGraphicsRootSignature(signature.Get()); // set the root signature

	for (const std::weak_ptr<DescriptorTable>& dt_wp : descriptor_tables) {
		if (const auto& dt = dt_wp.lock())
			dt->set_descriptor_table(device, command_list, descriptor_heap);
	}

	for (const auto& constants_wp : root_constants) {
		if (const auto& constants = constants_wp.lock()) constants->set_constants(command_list);
	}
}

bool GraphicsPipeline::RootSignature::operator==(const RootSignature& root_signature) const noexcept {
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

void GraphicsPipeline::RootSignature::create_views(const ComPtr<ID3D12Device>& device, GraphicsResourceDescriptorHeap& descriptor_heaps) {
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