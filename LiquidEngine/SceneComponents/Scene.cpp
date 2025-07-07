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

void Scene::add_object(const std::shared_ptr<Object>& object) noexcept {
	objects.insert(object);
	object->scene = this;
}

void Scene::add_character(const std::shared_ptr<Character> &character) noexcept {
	input_listener.keybind_sets.push_back(&character->keybind_set);
	objects.insert(std::static_pointer_cast<Object>(character));
	character->scene = this;
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
