#pragma once

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

	void compile() noexcept override;

	GET const Mesh & get_mesh() noexcept;
	void set_mesh(const Mesh &mesh) noexcept;

	GET Material & get_material() noexcept;
	void set_material(const Material &material) noexcept;

	bool operator==(const StaticMeshComponent &mesh) const noexcept;
	void operator=(const StaticMeshComponent &component) noexcept;

	static const Type component_type = Type::StaticMeshComponent;

private:
	friend class Renderer;

	Mesh mesh;
	Material material{};

	std::shared_ptr<GraphicsPipelineMeshChange::Manager> proxy = std::make_shared<GraphicsPipelineMeshChange::Manager>();
};
