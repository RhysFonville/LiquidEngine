#pragma once

#include "GraphicsPipeline.h"
#include "MeshComponent.h"

using ShaderType = GraphicsPipeline::Shader::Type;

class AppearanceComponent : public Component {
public:
	AppearanceComponent() { }
	AppearanceComponent(const std::shared_ptr<MeshComponent> &mesh);
	AppearanceComponent(const Material &material);
	AppearanceComponent(const GraphicsPipeline &pipeline,
		const std::shared_ptr<MeshComponent> &mesh = nullptr,
		const Material &material = Material());

	void compile(ComPtr<ID3D12Device> &device,
		ComPtr<ID3D12GraphicsCommandList> &command_list,
		const DXGI_SAMPLE_DESC &sample_desc,
		const UVector2 &resolution) noexcept;

	void clean_up() override;

	void set_mesh(const std::shared_ptr<MeshComponent> &mesh,
		ComPtr<ID3D12Device> &device,
		ComPtr<ID3D12GraphicsCommandList> &command_list) noexcept;

	bool mesh_was_changed() const noexcept;

	template <typename T>
	GraphicsPipeline::RootSignature::ConstantBuffer add_constant_buffer(const T &cb, ShaderType shader) {
		pipeline.root_signature.add_constant_buffer(cb, (D3D12_SHADER_VISIBILITY)shader+1));
	}

	bool operator==(const AppearanceComponent &appearance) const noexcept;
	void operator=(const AppearanceComponent &component) noexcept;

	GraphicsPipeline pipeline;
	Material material;

	static const Type component_type = Type::AppearanceComponent;

private:
	mutable bool mesh_changed = false;

	std::shared_ptr<MeshComponent> mesh = nullptr;
};
