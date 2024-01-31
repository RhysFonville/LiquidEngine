#include "GraphicsScene.h"

void GraphicsScene::clean_up() {
	for (RenderingStaticMesh &mesh : static_meshes) {
		mesh.mesh->clean_up();
	}
}
