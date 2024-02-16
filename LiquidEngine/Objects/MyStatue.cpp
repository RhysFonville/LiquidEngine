#include "MyStatue.h"

MyStatue::MyStatue() : mesh{std::make_shared<StaticMeshComponent>(Mesh{"statue.obj"})} {
	mesh->set_size(FVector3(0.3f, 0.3f, 0.3f));
}

void MyStatue::on_start() {
	add_component(mesh);
}

void MyStatue::tick(float dt) {
	rotate(FVector3{1.0f, 2.0f, 3.0f}*dt);
	/*if (timer % 10000 == 0) {
		Material material = mesh->get_material();
		material.set_albedo_texture(reg_tex ? "mipmaptest.png" : "texture.png");
		mesh->set_material_data(material);
		timer = 0;
		reg_tex = !reg_tex;
	}*/
	timer++;
}
