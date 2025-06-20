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
	StaticMeshComponent(const std::map<float, Mesh>& meshes = {{0.0f, Mesh{}}}, const Material& mat = Material{}, const std::vector<Transform>& instances = {Transform{}});
	StaticMeshComponent(const Mesh& mesh = Mesh{}, const Material& mat = Material{}, const std::vector<Transform>& instances = {Transform{}});

	void clean_up() override;

	void compile() noexcept override;

	GET const Mesh & get_mesh() noexcept;
	void set_mesh(const Mesh &mesh) noexcept;

	GET const std::map<float, Mesh>& get_lod_meshes() const noexcept;
	void set_lod_meshes(std::map<float, Mesh>& meshes) noexcept;

	GET std::map<float, Mesh>::const_iterator& get_current_mesh() noexcept;

	std::map<float, Mesh>::const_iterator get_mesh_for_point(const FVector3& pos) noexcept;

	GET const std::weak_ptr<Material>& get_material() noexcept;
	//void set_material(const Material &material) noexcept;

	//void set_instances(const std::vector<Transform>& mesh) noexcept;

	bool operator==(const StaticMeshComponent &mesh) const noexcept;
	void operator=(const StaticMeshComponent &component) noexcept;

private:
	void render_editor_gui_section() override;

	void fill_mesh_commands();

	std::map<float, Mesh> meshes{};
	std::map<float, Mesh>::iterator current_mesh{};
	
	std::shared_ptr<Material> material{};

	std::vector<Transform> instances{Transform{}};
};
