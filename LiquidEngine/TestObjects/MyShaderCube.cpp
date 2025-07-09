#include "MyShaderCube.h"
#include "../Core/Scene.h"

MyShaderCube::MyShaderCube() : Object{} { }

void MyShaderCube::pre_scene_compile() {
	mesh = static_cast<StaticMeshComponent*>(add_component(std::make_unique<StaticMeshComponent>(
		std::map<float, Mesh>{
			{0.0f, Mesh{"lodstatue4.obj"}},
				//std::make_pair<float, Mesh>(30.0f, Mesh{"lodstatue2.obj"}),
				//std::make_pair<float, Mesh>(50.0f, Mesh{"lodstatue3.obj"}),
				//std::make_pair<float, Mesh>(90.0f, Mesh{"lodstatue4.obj"}),
			{std::make_pair<float, Mesh>(130.0f, Mesh{})}
	},
		Material{"Graphics/Shaders/DefaultVertex.hlsl", "Graphics/Shaders/MyPixelShader.hlsl"}
	)));

	/*std::vector<Transform> instances{};
	for (int x = -20; x <= 20; x++) {
		for (int y = -20; y <= 20; y++) {
			for (int z = -20; z <= 20; z++) {
				instances.push_back(Transform{FVector3{(float)x*3, (float)y*3, (float)z*3}});
			}
		}
	}

	mesh->set_instances(instances);*/

	//mesh->get_material().get_albedo_texture().set_texture("cratealbedo.png");
	//mesh->get_material().get_specular_map().set_texture("cratespecular.png");

	box = static_cast<BoundingBoxComponent*>(add_component(std::make_unique<BoundingBoxComponent>()));

	mimic_position_component = box;
	mimic_rotation_component = box;

	child = add_object(std::make_unique<Object>());
	child->translate(FVector3{0.f, 0.f, 3.f});
	child->add_component(std::make_unique<StaticMeshComponent>(Mesh{"Shapes/cube.obj"}, Material{"Graphics/Shaders/DefaultVertex.hlsl", "Graphics/Shaders/MyPixelShader.hlsl"}));
}

void MyShaderCube::post_scene_compile() {
	rc.set_rc(mesh->get_material().lock()->pipeline.root_signature.get_root_constants("WAVE_CONSTANTS"));
}

void MyShaderCube::tick(float dt) {
	rc.get_obj()->time += dt;
}

void MyShaderCube::render_editor_gui_section() {
	ImGui::InputFloat("Amplitude", &rc.get_obj()->amplitude);
	ImGui::InputFloat("Frequency", &rc.get_obj()->frequency);
	ImGui::InputFloat("Phase", &rc.get_obj()->phase);
}
