#include "MyShaderCube.h"

MyShaderCube::MyShaderCube()
	: mesh{std::make_shared<StaticMeshComponent>(
		std::map<float, Mesh>{
			//std::make_pair<float, Mesh>(0.0f, Mesh{"lodstatue1.obj"}),
			//std::make_pair<float, Mesh>(30.0f, Mesh{"lodstatue2.obj"}),
			std::make_pair<float, Mesh>(50.0f, Mesh{"lodstatue3.obj"}),
			std::make_pair<float, Mesh>(70.0f, Mesh{"lodstatue4.obj"}),
			std::make_pair<float, Mesh>(80.0f, Mesh{})
		},
		Material{"Graphics/Shaders/DefaultVertex.hlsl", "Graphics/Shaders/MyPixelShader.hlsl"})
	},
	box{std::make_shared<BoundingBoxComponent>()} {
	
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

	mimic_position_component = box.get();
	mimic_rotation_component = box.get();
}

void MyShaderCube::pre_scene_compile() {
	add_component(mesh);
	add_component(box);
}

void MyShaderCube::post_scene_compile() {
	rc.set_rc(mesh->get_material().pipeline.root_signature.get_root_constants("WAVE_CONSTANTS"));
}

void MyShaderCube::tick(float dt) {
	rc.get_obj()->time += dt;
}

void MyShaderCube::render_editor_gui_section() {
	ImGui::InputFloat("Amplitude", &rc.get_obj()->amplitude);
	ImGui::InputFloat("Frequency", &rc.get_obj()->frequency);
	ImGui::InputFloat("Phase", &rc.get_obj()->phase);
}
