#include "GraphicsScene.h"

void GraphicsScene::clean_up() {
	for (StaticMeshComponent* mesh : static_meshes) {
		mesh->clean_up();
	}
}
