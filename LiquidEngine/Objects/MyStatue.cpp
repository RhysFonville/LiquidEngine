#include "MyStatue.h"

MyStatue::MyStatue() : mesh{std::make_shared<StaticMeshComponent>(Mesh{"Shapes/cube.obj"})} {
	mesh->set_size(FVector3(0.3f, 0.3f, 0.3f));
}

void MyStatue::on_start() {
	add_component(mesh);
}

void MyStatue::tick(float dt) {
	rotate(FVector3{1.0f, 2.0f, 3.0f}*dt);
}
