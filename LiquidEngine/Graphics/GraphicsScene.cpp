#include "GraphicsScene.h"

void GraphicsScene::clean_up() {
	for (auto &mesh : static_meshes) {
		mesh->clean_up();
		mesh->component->clean_up();
	}

	camera.clean_up();
	camera.component->clean_up();

	sky.clean_up();
	if (sky.component != nullptr)
		sky.component->clean_up();
}
