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

static bool ends_with(std::string const& value, std::string const& ending) {
	if (ending.size() > value.size()) return false;
	return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
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
			if (ends_with(shader_input_bind_desc.Name, "_CONSTANTS")) {
				ID3D12ShaderReflectionConstantBuffer* buffer = reflection->GetConstantBufferByIndex(i);
				D3D12_SHADER_BUFFER_DESC buffer_desc{};
				buffer->GetDesc(&buffer_desc);

				std::shared_ptr<RootConstants> rc{std::make_shared<RootConstants>(
					RootConstants{shader_input_bind_desc, buffer_desc, (UINT)get_number_of_params()}
				)};

				root_constants.insert(
					std::make_pair(std::string{shader_input_bind_desc.Name}, rc)
				);
				root_params.push_back((RootArgument*)rc.get());
			} else {
				std::shared_ptr<ConstantBuffer> cb{std::make_shared<ConstantBuffer>(
					ConstantBuffer{
						DescriptorTable{
							shader_input_bind_desc,
							D3D12_DESCRIPTOR_RANGE_TYPE_CBV,
							(UINT)get_number_of_params()
						}
					}
				)};

				constant_buffers.insert(
					std::make_pair(std::string{shader_input_bind_desc.Name}, cb)
				);
				root_params.push_back((RootArgument*)&cb->descriptor_table);
			}
		} else if (shader_input_bind_desc.Type == D3D_SIT_TEXTURE) {
			std::shared_ptr<ShaderResourceView> srv{std::make_shared<ShaderResourceView>(
				ShaderResourceView{
					DescriptorTable{
						shader_input_bind_desc,
						D3D12_DESCRIPTOR_RANGE_TYPE_SRV,
						(UINT)get_number_of_params()
					}
				}
			)};
			
			shader_resource_views.insert(
				std::make_pair(std::string{shader_input_bind_desc.Name}, srv)
			);
			root_params.push_back((RootArgument*)&srv->descriptor_table);
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
	rm(root_constants);

	root_params.clear();
	signature.Reset();
}

void GraphicsPipeline::RootSignature::check_for_update(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heap) {
	command_list->SetGraphicsRootSignature(signature.Get());
	
	std::vector<std::map<std::string, std::shared_ptr<ConstantBuffer>>::iterator> set_cb{};
	set_cb.reserve(constant_buffers.size());

	for (auto it{constant_buffers.begin()}; it != constant_buffers.end(); it++) {
		if (it->second->will_compile() && !it->second->is_null()) {
			it->second->compile(device, command_list, descriptor_heap);
			set_cb.push_back(it);
		}
	}

	for (auto it{constant_buffers.begin()}; it != constant_buffers.end(); it++) {
		if (it->second->will_update()) {
			it->second->update(device, command_list);
			set_cb.push_back(it);
		}
	}

	for (auto it : set_cb)
		it->second->descriptor_table.set_descriptor_table(device, command_list, descriptor_heap);

	for (const std::pair<std::string, std::shared_ptr<ShaderResourceView>>& srv : shader_resource_views) {
		if (srv.second->will_compile() && !srv.second->is_null()) {
			srv.second->compile(device, command_list, descriptor_heap);
			srv.second->descriptor_table.set_descriptor_table(device, command_list, descriptor_heap);
		}
	}
}

void GraphicsPipeline::RootSignature::run(const ComPtr<ID3D12Device>& device, const ComPtr<ID3D12GraphicsCommandList>& command_list, GraphicsResourceDescriptorHeap& descriptor_heap) {
	for (const std::pair<std::string, std::shared_ptr<RootConstants>>& constants : root_constants) {
		constants.second->set_constants(command_list);
	}
}

bool GraphicsPipeline::RootSignature::operator==(const RootSignature& root_signature) const noexcept {
	return (signature == root_signature.signature);
}

void GraphicsPipeline::RootSignature::create_views(const ComPtr<ID3D12Device>& device, GraphicsResourceDescriptorHeap& descriptor_heaps) {
	for (const auto& cb : constant_buffers) {
		cb.second->create_view(device, descriptor_heaps);
	}
	for (const auto& srv : shader_resource_views) {
		srv.second->create_view(device, descriptor_heaps);
	}
}

/*std::vector<DescriptorTable> GraphicsPipeline::RootSignature::get_descriptor_tables() const noexcept {
	std::vector<DescriptorTable> dts{};
	dts.reserve(descriptor_tables.size());
	for (const DescriptorTable* dt : descriptor_tables) {
		dts.push_back(*dt);
	}

	return dts;
}*/

std::vector<D3D12_ROOT_PARAMETER1> GraphicsPipeline::RootSignature::get_root_params() const noexcept {
	std::vector<D3D12_ROOT_PARAMETER1> params{};
	for (const auto& param : root_params) {
		params.push_back(param->get_root_param());
	}

	return params;
}

std::weak_ptr<DescriptorRootObject> GraphicsPipeline::RootSignature::get_resource(const std::string& name) const {
	auto cb{constant_buffers.find(name)};
	auto srv{shader_resource_views.find(name)};

	if (cb != constant_buffers.end()) {
		return std::static_pointer_cast<DescriptorRootObject>(cb->second);
	} else if (srv != shader_resource_views.end()) {
		return std::static_pointer_cast<DescriptorRootObject>(srv->second);
	} else {
		throw std::exception{"Can't find root signature resource of that name."};
		return std::weak_ptr<DescriptorRootObject>{};
	}
}

std::weak_ptr<ConstantBuffer> GraphicsPipeline::RootSignature::get_constant_buffer(const std::string& name) const {
	if (ends_with(name, "_CONSTANTS")) {
		throw std::exception{"Only root constants can have a name that ends with '_CONSTANTS'."};
		return std::weak_ptr<ConstantBuffer>{};
	}
	
	if (auto cb{constant_buffers.find(name)}; cb != constant_buffers.end()) {
		return cb->second;
	} else {
		throw std::exception{"Can't find constant buffer of that name."};
		return std::weak_ptr<ConstantBuffer>{};
	}
}

std::weak_ptr<RootConstants> GraphicsPipeline::RootSignature::get_root_constants(const std::string& name) const {
	if (!ends_with(name, "_CONSTANTS")) {
		throw std::exception{"Root constants name must end with '_CONSTANTS'."};
		return std::weak_ptr<RootConstants>{};
	}
	
	if (auto rc{root_constants.find(name)}; rc != root_constants.end()) {
		return rc->second;
	} else {
		throw std::exception{"Can't find root constants of that name."};
		return std::weak_ptr<RootConstants>{};
	}
}

std::weak_ptr<ShaderResourceView> GraphicsPipeline::RootSignature::get_shader_resource_view(const std::string& name) const {
	if (ends_with(name, "_CONSTANTS")) {
		throw std::exception{"Only root constants can have a name that ends with '_CONSTANTS'."};
		return std::weak_ptr<ShaderResourceView>{};
	}
	
	if (auto srv{shader_resource_views.find(name)}; srv != shader_resource_views.end()) {
		return srv->second;
	} else {
		throw std::exception{"Can't find shader resource view of that name."};
		return std::weak_ptr<ShaderResourceView>{};
	}
}

