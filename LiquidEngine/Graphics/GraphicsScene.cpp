#include "GraphicsScene.h"

void GraphicsScene::clean_up() {
	for (RenderingStaticMesh &mesh : static_meshes) {
		mesh.component->clean_up();
	}
	for (RenderingDirectionalLight &dl : directional_lights) {
		dl.component->clean_up();
	}
	for (RenderingPointLight &pl : point_lights) {
		pl.component->clean_up();
	}
	for (RenderingSpotlight &sl : spotlights) {
		sl.component->clean_up();
	}
}
