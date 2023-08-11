#include "DefaultCube.h"

DefaultCube::DefaultCube() : mesh(std::make_shared<StaticMeshComponent>()) { }

void DefaultCube::on_start() {
	add_component(mesh);
}
