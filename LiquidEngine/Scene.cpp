#include "Scene.h"

Scene::Scene(GraphicsScene* graphics_scene) : graphics_scene(graphics_scene) { }

void Scene::tick(float dt) {
	render_editor_gui_section();

	input_listener.handle_input(dt);

	physics_scene.tick(dt);

	for (std::shared_ptr<Object> &object : objects) {
		object->base_tick(dt);
	}
}

void Scene::clean_up() {
	physics_scene.clean_up();
}

void Scene::compile() {
	for (std::shared_ptr<Object> &object : objects) {
		object->base_compile();
	}
}

std::vector<std::shared_ptr<Object>> & Scene::get_objects() noexcept {
	return objects;
}

void Scene::add_object(const std::shared_ptr<Object> &object) noexcept {
	objects.push_back(object);
	objects.back()->graphics_scene = graphics_scene;
	objects.back()->physics_scene = &physics_scene;
}

void Scene::add_character(const std::shared_ptr<Character> &character) noexcept {
	input_listener.keybind_sets.push_back(&character->keybind_set);
	objects.push_back(std::static_pointer_cast<Object>(character));
	objects.back()->graphics_scene = graphics_scene;
	objects.back()->physics_scene = &physics_scene;
}

void Scene::remove_object(int index) noexcept {
	objects.erase(objects.begin()+index);
}

void Scene::render_editor_gui_section() {
	if (ImGui::Button("Add object")) {
		add_object(std::make_shared<Object>());
		objects.back()->add_component(std::make_shared<StaticMeshComponent>(Mesh{"Shapes/cube.obj"}));
		objects.back()->add_component(std::make_shared<BoundingBoxComponent>());
		objects.back()->mimic_position_component = objects.back()->get_component<BoundingBoxComponent>();
		objects.back()->base_compile();
	}

	ImGui::Text("Objects");
	int i = 0;
	for (auto &object : objects) {
		if (ImGui::TreeNode(("Object" + std::to_string(i)).c_str())) {
			ImGui::InputText("Name", &object->name);
			object->base_render_editor_gui_section();
			ImGui::TreePop();
		}
		i++;
	}
}
