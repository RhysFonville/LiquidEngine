#include "Scene.h"

Scene::Scene(const std::shared_ptr<Window> &window) {
	objects = std::make_shared<std::vector<Object>>(std::vector<Object>());
	objects->reserve(1000);
	graphics_scene = GraphicsScene(*window->get_window(), objects);
	physics_scene = PhysicsScene();
	behavior_manager = BehaviorManager();
}

void Scene::tick() {
	behavior_manager.tick();
	graphics_scene.clear();
	graphics_scene.draw();
	graphics_scene.present();
}
