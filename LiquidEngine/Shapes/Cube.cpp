#include "Cube.h"

namespace DefaultShapes {

Cube::Cube() {
	mesh = static_cast<StaticMeshComponent*>(add_component(std::make_unique<StaticMeshComponent>(Mesh{"Shapes/cube.obj"})));
}

};
