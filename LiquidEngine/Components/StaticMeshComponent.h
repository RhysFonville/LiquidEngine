#pragma once

#include "../Graphics/Material.h"
#include "../imgui/imgui_stdlib.h"

/**
 * Main component for any physical object with a mesh that will be rendered. Has mesh data and a material.
 * \see Mesh
 * \see Material
 */
class StaticMeshComponent : public GraphicsComponent {
public:
	StaticMeshComponent(const Mesh& mesh = Mesh{}, const Material& mat = Material{}, const std::vector<Transform>& instances = {Transform{}});

	void clean_up() override;

	void compile() noexcept override;

	GET const Mesh & get_mesh() noexcept;
	void set_mesh(const Mesh &mesh) noexcept;

	GET Material & get_material() noexcept;
	//void set_material(const Material &material) noexcept;

	void set_instances(const std::vector<Transform>& mesh) noexcept;

	bool operator==(const StaticMeshComponent &mesh) const noexcept;
	void operator=(const StaticMeshComponent &component) noexcept;

	static const Type component_type = Type::StaticMeshComponent;

private:
	void render_editor_gui_section() override;

	Mesh mesh{};
	Material material{};

	std::vector<Transform> instances{Transform{}};
};
