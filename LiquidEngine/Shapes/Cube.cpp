#include "Cube.h"

namespace DefaultShapes {

Cube::Cube()
	: mesh(std::make_shared<StaticMeshComponent>(Mesh("Shapes/cube.obj"))) { }

void Cube::pre_scene_compile() {
	add_component(mesh);
}

};
