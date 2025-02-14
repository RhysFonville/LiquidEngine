#include "GraphicsScene.h"

void GraphicsScene::clean_up() {
	for (auto &mesh : static_meshes) {
		mesh->clean_up();
	}

	camera.clean_up();
	sky.clean_up();
}
