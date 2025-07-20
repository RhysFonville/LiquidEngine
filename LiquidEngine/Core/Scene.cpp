#include "Scene.h"

Scene::Scene(GraphicsScene* graphics_scene) : graphics_scene(graphics_scene) { }

void Scene::tick(float dt) {
	render_editor_gui_section();

	input_listener.handle_input(dt);

	//physics_scene.tick(dt);

	for (const std::unique_ptr<Object>& object : objects) {
		object->base_tick(dt);
	}
}

void Scene::clean_up() {
	//physics_scene.clean_up();
}

void Scene::compile() {
	for (const std::unique_ptr<Object>& object : objects) {
		object->base_compile();
	}
}

std::set<Object*> Scene::get_objects() const noexcept {
	return objects | std::views::transform([](const auto& obj){ return obj.get(); }) | std::ranges::to<std::set>();
}

Object* Scene::add_object(std::unique_ptr<Object>&& obj) {
	obj->set_scene(this);
	return objects.insert(std::move(obj)).first->get();
}

Character* Scene::add_character(std::unique_ptr<Character>&& character) {
	input_listener.keybind_sets.push_back(&character->keybind_set);
	character->set_scene(this);
	return static_cast<Character*>(objects.insert(std::move(character)).first->get());
}

void Scene::render_editor_gui_section() {
	static Object* selected_object{nullptr};

	if (ImGui::BeginTable("1way", 1)) {
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
		ImGui::TableHeadersRow();


		size_t depth{0};
		for (auto& obj : objects) {
			if (depth == 0)
				obj->base_render_editor_gui_section_tree(&selected_object);
			else depth--;
			depth += obj->get_children_count();
		}

		ImGui::EndTable();
	}

	if (selected_object != nullptr) selected_object->base_render_editor_gui_section();
}
