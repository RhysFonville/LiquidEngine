#include "MyStatue.h"

MyStatue::MyStatue() {
	mesh = static_cast<StaticMeshComponent*>(add_component(std::make_unique<StaticMeshComponent>(Mesh{"statue.obj"})));
	mesh->set_size(FVector3(0.3f, 0.3f, 0.3f));
}

void MyStatue::tick(float dt) {
	rotate(FVector3{1.0f, 2.0f, 3.0f}*dt);
	if (timer % 10000 == 0) {
		//mat->get_albedo_texture().set_texture(reg_tex ? "texture.png" : "mipmaptest.png");
		timer = 0;
		reg_tex = !reg_tex;
	}
	timer++;
}
