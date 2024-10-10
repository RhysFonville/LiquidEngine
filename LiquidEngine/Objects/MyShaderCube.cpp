#include "MyShaderCube.h"

MyShaderCube::MyShaderCube()
	: mesh{std::make_shared<StaticMeshComponent>(Mesh{"crate.obj"}, Material{"Graphics/DefaultVertex.hlsl", "Graphics/MyPixelShader.hlsl"})},
	box{std::make_shared<BoundingBoxComponent>()} {
	
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
