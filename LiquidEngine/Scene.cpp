#include "Scene.h"

Scene::Scene(GraphicsScene* graphics_scene) : graphics_scene(graphics_scene) { }

void Scene::tick(float dt) {
	for (std::shared_ptr<Object> &object : objects) {
		object->base_tick(dt);
	}

	//physics_scene.tick();
}

void Scene::clean_up() {
	//graphics_scene.clean_up();
	//physics_scene.clean_up();
}

void Scene::compile() {
	for (std::shared_ptr<Object> &object : objects) {
		object->compile();
	}
}

std::vector<std::shared_ptr<Object>> & Scene::get_objects() noexcept {
	return objects;
}

void Scene::add_object(const std::shared_ptr<Object> &object) noexcept {
	objects.push_back(object);
	objects.back()->graphics_scene = graphics_scene;
	objects.back()->on_start();
}

void Scene::remove_object(int index) noexcept {
	objects.erase(objects.begin()+index);
}
