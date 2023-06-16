#pragma once

#include "GraphicsPipeline.h"
#include "MeshComponent.h"
#include "Material.h"

using ShaderType = GraphicsPipeline::Shader::Type;

class AppearanceComponent : public Component {
public:
	AppearanceComponent() { }
	AppearanceComponent(MeshComponent* mesh);
	AppearanceComponent(const Material &material);
	AppearanceComponent(const GraphicsPipeline &pipeline,
		MeshComponent *mesh = nullptr,
		const Material &material = Material());

	void compile(ComPtr<ID3D12Device> &device,
		ComPtr<ID3D12GraphicsCommandList> &command_list,
		const DXGI_SAMPLE_DESC &sample_desc,
		const UVector2 &resolution) noexcept;

	void clean_up() override;

	const MeshComponent* get_mesh() const noexcept;

	void set_mesh(MeshComponent* mesh,
		ComPtr<ID3D12Device> &device,
		ComPtr<ID3D12GraphicsCommandList> &command_list) noexcept;

	bool operator==(const AppearanceComponent &appearance) const noexcept;
	void operator=(const AppearanceComponent &component) noexcept;

	GraphicsPipeline pipeline;
	Material material;

	static const Type component_type = Type::AppearanceComponent;

private:
	MeshComponent* mesh = nullptr;
};
