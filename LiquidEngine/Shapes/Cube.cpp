#include "Cube.h"

namespace DefaultShapes {

Cube::Cube()
	: mesh(std::make_shared<StaticMeshComponent>(Mesh("Shapes/Cube.obj"))) { }

void Cube::on_start() {
	add_component(mesh);
}

};
