#pragma once

#include "GraphicsComponent.h"
#include "../Graphics/Material.h"

/**
 * Main component for any physical object with a mesh that will be rendered. Has mesh data and a material.
 * \see Mesh
 * \see Material
 */
class StaticMeshComponent : public GraphicsComponent {
public:
	StaticMeshComponent(Mesh mesh = Mesh());

	void clean_up() override;

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc, const UVector2 &resolution) noexcept;
	void compile() noexcept override;

	GET const Mesh & get_mesh() const noexcept;
	void set_mesh(const Mesh &mesh) noexcept;

	GET const Material & get_material() const noexcept;
	void set_material(const Material &material) noexcept;
	void set_material_data(const Material &material) noexcept;

	bool operator==(const StaticMeshComponent &mesh) const noexcept;
	void operator=(const StaticMeshComponent &component) noexcept;

	static const Type component_type = Type::StaticMeshComponent;

private:
	friend class Renderer;

	Mesh mesh;
	Material material;

	std::shared_ptr<GraphicsPipelineMeshChange::Manager> proxy = std::make_shared<GraphicsPipelineMeshChange::Manager>();
};
