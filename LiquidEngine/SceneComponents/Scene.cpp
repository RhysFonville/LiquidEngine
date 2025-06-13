#include "Scene.h"

Scene::Scene(GraphicsScene* graphics_scene) : graphics_scene(graphics_scene) { }

void Scene::tick(float dt) {
	render_editor_gui_section();

	input_listener.handle_input(dt);

	//physics_scene.tick(dt);

	for (const std::shared_ptr<Object>& object : objects) {
		object->base_tick(dt);
	}
}

void Scene::clean_up() {
	//physics_scene.clean_up();
}

void Scene::compile() {
	for (const std::shared_ptr<Object>& object : objects) {
		object->base_compile();
	}
}

std::unordered_set<std::shared_ptr<Object>> & Scene::get_objects() noexcept {
	return objects;
}

void Scene::add_object(const std::shared_ptr<Object> &object) noexcept {
	objects.insert(object);
	object->scene = this;
	object->set_parent(nullptr);
}

void Scene::add_character(const std::shared_ptr<Character> &character) noexcept {
	input_listener.keybind_sets.push_back(&character->keybind_set);
	objects.insert(std::static_pointer_cast<Object>(character));
	character->scene = this;
	character->set_parent(nullptr);
}

void Scene::render_editor_gui_section() {
	ImGui::Text("Objects");
	int i = 0;
	for (auto &object : objects) {
		if (ImGui::TreeNode(object->name.empty() ? "Unnamed object" : object->name.c_str())) {
			object->base_render_editor_gui_section();
			ImGui::TreePop();
		}
		i++;
	}
}
