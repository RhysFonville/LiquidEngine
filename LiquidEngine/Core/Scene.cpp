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

std::set<Object*> Scene::get_objects() const noexcept {
	return objects | std::views::transform([](const auto& obj){ return obj.get(); }) | std::ranges::to<std::set>();
}

Object* Scene::add_object(std::unique_ptr<Object>&& obj) {
	obj->set_scene(this);
	return objects.insert(std::move(obj)).first->get();
}

void Scene::add_character(std::unique_ptr<Character>&& character) noexcept {
	input_listener.keybind_sets.push_back(&character->keybind_set);
	character->set_scene(this);
	return objects.insert(std::move(obj)).first->get();
}

void Scene::render_editor_gui_section() {
	if (ImGui::BeginTable("1way", 1)) {
		ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_NoHide);
		ImGui::TableHeadersRow();

		std::vector<ObjectsTreeNode> nodes{};
		for (auto& object : objects) {
			object->base_render_editor_gui_section(nodes);
		}

		if (!nodes.empty()) {
			int depth{0};
			for (int i{0}; i < nodes.size(); i++) {
				if (depth == 0)
					ObjectsTreeNode::display_node(&nodes[i], nodes);
				if (nodes[i].child_count > 0) depth++;
			}
			object_name_index = 0;
		}

		ImGui::EndTable();
	}
}
