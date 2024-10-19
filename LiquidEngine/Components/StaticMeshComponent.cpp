#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent(const Mesh& mesh, const Material& mat, const std::vector<Transform>& instances)
	: GraphicsComponent{Type::StaticMeshComponent}, mesh{mesh}, material{mat}, instances{instances} { }

void StaticMeshComponent::compile() noexcept {
	material.pipeline.input_assembler.clear_commands();
	material.pipeline.input_assembler.add_command(std::make_shared<GraphicsPipelineIARemoveAllMeshesCommand>());
	material.pipeline.input_assembler.add_command(std::make_shared<GraphicsPipelineIAAddMeshCommand>(std::make_shared<Mesh>(mesh)));
	material.pipeline.input_assembler.add_command(std::make_shared<GraphicsPipelineIASetInstancesCommand>(instances));
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

	material.pipeline.input_assembler.add_command(std::make_shared<GraphicsPipelineIARemoveAllMeshesCommand>());
	material.pipeline.input_assembler.add_command(std::make_shared<GraphicsPipelineIAAddMeshCommand>(std::make_shared<Mesh>(mesh)));

	changed = true;
}

Material & StaticMeshComponent::get_material() noexcept {
	return material;
}

void StaticMeshComponent::set_instances(const std::vector<Transform>& instances) noexcept {
	this->instances = instances;

	material.pipeline.input_assembler.add_command(std::make_shared<GraphicsPipelineIASetInstancesCommand>(instances));

	changed = true;
}

//void StaticMeshComponent::set_material(const Material &material) noexcept {
//	this->material = material;
//
//	proxy->remove_all_meshes();
//	proxy->add_mesh(mesh);
//
//	changed = true;
//}

bool StaticMeshComponent::operator==(const StaticMeshComponent &mesh) const noexcept {
	return ((Component*)this == (Component*)&mesh &&
		this->mesh == mesh.mesh);
}

void StaticMeshComponent::operator=(const StaticMeshComponent &component) noexcept {
	mesh = component.mesh;
}

void StaticMeshComponent::render_editor_gui_section() {
	std::string mesh{};
	if (ImGui::InputText("Mesh", &mesh, ImGuiInputTextFlags_EnterReturnsTrue)) {
		if (fs::exists(mesh))
			this->set_mesh(Mesh{mesh});
	}

	ImGui::Text("Material");

	static bool enable_mipmap{true};
	if (ImGui::Checkbox("Enable mipmap", &enable_mipmap)) {
		material.get_albedo_texture().set_texture(material.get_albedo_texture().get_file(), enable_mipmap);
	}

	std::string tex = material.get_albedo_texture().get_file();
	if (ImGui::InputText("Albedo texture", &tex, ImGuiInputTextFlags_EnterReturnsTrue)) {
		material.get_albedo_texture().set_texture(tex, enable_mipmap);
	}

	tex = material.get_normal_map().get_file();
	if (ImGui::InputText("Normal map", &tex, ImGuiInputTextFlags_EnterReturnsTrue)) {
		material.get_normal_map().set_texture(tex, enable_mipmap);
	}

	tex = material.get_specular_map().get_file();
	if (ImGui::InputText("Specular map", &tex, ImGuiInputTextFlags_EnterReturnsTrue)) {
		material.get_specular_map().set_texture(tex, enable_mipmap);
	}

	FVector4 alb{color_to_fvector(material.get_albedo())};
	FVector4 spc{color_to_fvector(material.get_specular())};
	FVector4 amb{color_to_fvector(material.get_ambient())};

	float col[4]{(float)alb.x, (float)alb.y, (float)alb.z, (float)alb.w};
	if (ImGui::ColorEdit4("Albedo", col))
		material.set_albedo(Color{UCHAR(col[0]*255.0f), UCHAR(col[1]*255.0f), UCHAR(col[2]*255.0f), UCHAR(col[3]*255.0f)});
	
	col[0] = spc.x;
	col[1] = spc.y;
	col[2] = spc.z;
	col[3] = spc.w;
	if (ImGui::ColorEdit4("Specular", col))
		material.set_specular(Color{UCHAR(col[0]*255.0f), UCHAR(col[1]*255.0f), UCHAR(col[2]*255.0f), UCHAR(col[3]*255.0f)});
	
	col[0] = amb.x;
	col[1] = amb.y;
	col[2] = amb.z;
	col[3] = amb.w;
	if (ImGui::ColorEdit4("Ambient", col))
		material.set_ambient(Color{UCHAR(col[0]*255.0f), UCHAR(col[1]*255.0f), UCHAR(col[2]*255.0f), UCHAR(col[3]*255.0f)});
	
	float shi{material.get_shininess()};
	if (ImGui::InputFloat("Shininess", &shi))
		material.set_shininess(shi);
}
