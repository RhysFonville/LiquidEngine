#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent(Mesh mesh)
	: GraphicsComponent(Type::StaticMeshComponent), mesh(mesh) {

	material.pipeline.input_assembler.set_proxy(proxy);
}

void StaticMeshComponent::compile() noexcept {
	proxy->remove_all_meshes();
	proxy->add_mesh(mesh);
	material.compile();
}

void StaticMeshComponent::clean_up() {
	material.clean_up();
}

const Mesh & StaticMeshComponent::get_mesh() noexcept {
	return mesh;
}

void StaticMeshComponent::set_mesh(const Mesh &mesh) noexcept {
	this->mesh = mesh;
	this->mesh.compile();

	proxy->remove_all_meshes();
	proxy->add_mesh(mesh);

	changed = true;
}

Material & StaticMeshComponent::get_material() noexcept {
	return material;
}

void StaticMeshComponent::set_material(const Material &material) noexcept {
	this->material = material;
	this->material.pipeline.input_assembler.set_proxy(proxy);
	proxy->remove_all_meshes();
	proxy->add_mesh(mesh);

	changed = true;
}

bool StaticMeshComponent::operator==(const StaticMeshComponent &mesh) const noexcept {
	return ((Component*)this == (Component*)&mesh &&
		this->mesh == mesh.mesh);
}

void StaticMeshComponent::operator=(const StaticMeshComponent &component) noexcept {
	mesh = component.mesh;
}

void StaticMeshComponent::render_editor_gui_section() {
	std::string mesh{};
	if (ImGui::InputText("Mesh", &mesh[0], mesh.max_size())) {
		this->mesh.set_vertices(mesh);
	}

	ImGui::Text("Material");

	std::string tex{material.get_albedo_texture().get_file()};
	if (ImGui::InputText("Albedo texture", &tex[0], tex.max_size())) {
		material.get_albedo_texture().set_texture(tex);
	}

	tex = material.get_normal_map().get_file();
	if (ImGui::InputText("Normal map", &tex[0], tex.max_size())) {
		material.get_normal_map().set_texture(tex);
	}

	tex = material.get_environment_texture().get_file();
	if (ImGui::InputText("Environment texture", &tex[0], tex.max_size())) {
		material.get_environment_texture().set_texture(tex);
	}

	Color alb{material.get_albedo()};
	Color spc{material.get_specular()};
	Color amb{material.get_ambient()};

	float col[4]{(float)alb.r, (float)alb.g, (float)alb.b, (float)alb.a};
	if (ImGui::ColorEdit4("Albedo", col))
		material.set_albedo(Color{(UCHAR)col[0], (UCHAR)col[1], (UCHAR)col[2], (UCHAR)col[3]});
	
	col[0] = spc.r;
	col[1] = spc.g;
	col[2] = spc.b;
	col[3] = spc.a;
	if (ImGui::ColorEdit4("Specular", col))
		material.set_specular(Color{(UCHAR)col[0], (UCHAR)col[1], (UCHAR)col[2], (UCHAR)col[3]});
	
	col[0] = amb.r;
	col[1] = amb.g;
	col[2] = amb.b;
	col[3] = amb.a;
	if (ImGui::ColorEdit4("Ambient", col))
		material.set_ambient(Color{(UCHAR)col[0], (UCHAR)col[1], (UCHAR)col[2], (UCHAR)col[3]});
	
	float shi{material.get_shininess()};
	if (ImGui::InputFloat("Shininess", &shi))
		material.set_shininess(shi);
}
