#include "MyShaderCube.h"

MyShaderCube::MyShaderCube()
	: mesh{std::make_shared<StaticMeshComponent>(Mesh{"crate.obj"}, Material{"Graphics/Shaders/DefaultVertex.hlsl", "Graphics/Shaders/MyPixelShader.hlsl"})},
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

	mesh->get_material().get_albedo_texture().set_texture("cratealbedo.png");
	mesh->get_material().get_specular_map().set_texture("cratespecular.png");

	mimic_position_component = box.get();
	mimic_rotation_component = box.get();
}

void MyShaderCube::pre_scene_compile() {
	add_component(mesh);
	add_component(box);
}

void MyShaderCube::tick(float dt) {

}
