#include "MyStatue.h"

MyStatue::MyStatue() : mesh{std::make_shared<StaticMeshComponent>(Mesh{"statue.obj"})},
	mat{std::make_shared<MaterialComponent>()} {
	mesh->set_size(FVector3(0.3f, 0.3f, 0.3f));
	mesh->set_material(mat.get());
}

void MyStatue::on_start() {
	add_component(mesh);
	add_component(mat);
}

void MyStatue::tick(float dt) {
	//rotate(FVector3{1.0f, 2.0f, 3.0f}*dt);
	if (timer % 10000 == 0) {
		//mat->get_albedo_texture().set_texture(reg_tex ? "mipmaptest.png" : "texture.png");
		timer = 0;
		reg_tex = !reg_tex;
	}
	timer++;
}
