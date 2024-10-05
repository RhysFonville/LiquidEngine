#include "MyShaderCube.h"

MyShaderCube::MyShaderCube()
	: mesh{std::make_shared<StaticMeshComponent>(Mesh{"statue.obj"}, Material{"Graphics/DefaultVertex.hlsl", "Graphics/LitPixel.hlsl"})},
	box{std::make_shared<BoundingBoxComponent>()} {
	
	mesh->get_material().get_environment_texture().set_texture("Skybox.png");

	mimic_position_component = box.get();
	mimic_rotation_component = box.get();

	mesh->get_material().add_shader_argument(&time.rc);
}

void MyShaderCube::pre_scene_compile() {
	add_component(mesh);
	add_component(box);
}

void MyShaderCube::tick(float dt) {
	*time.obj += dt;
}
