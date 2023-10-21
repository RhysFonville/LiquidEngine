#pragma once

#include "PhysicalComponent.h"
#include "../Graphics/Material.h"

class StaticMeshComponent : public GraphicsComponent {
public:
	StaticMeshComponent(Mesh mesh = Mesh());

	void clean_up() override;

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const UVector2 &resolution) noexcept;
	void compile() noexcept override;

	GET Mesh get_mesh() const noexcept;
	void set_mesh(const Mesh &mesh) noexcept;

	GET Material & get_material() noexcept;
	void set_material(const Material &material) noexcept;

	bool operator==(const StaticMeshComponent &mesh) const noexcept;
	void operator=(const StaticMeshComponent &component) noexcept;

	static const Type component_type = Type::StaticMeshComponent;

private:
	Mesh mesh;
	Material material;

	std::shared_ptr<GraphicsPipelineMeshChangeManager> proxy = std::make_shared<GraphicsPipelineMeshChangeManager>();
};
