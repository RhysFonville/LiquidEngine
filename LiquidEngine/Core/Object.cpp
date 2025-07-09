#include "Scene.h"
#include "../Components/GraphicsComponent.h"

void Object::clean_components() {
	for (const std::unique_ptr<Component>& component : components) {
		component->base_clean_up();
	}
	components.clear();
}

void Object::base_clean_up() {
	children.clear();
	clean_components();
	clean_up();
}

void Object::base_compile() {
	for (const std::unique_ptr<Object>& child : children) {
		child->base_compile();
	}

	for (const std::unique_ptr<Component>& comp : components) {
		comp->base_compile();
	}

	compile();
}

void Object::base_tick(float dt) {
	for (const std::unique_ptr<Object>& child : children) {
		child->base_tick(dt);
	}

	for (const std::unique_ptr<Component>& comp : components) {
		comp->base_tick(dt);
	}

	if (mimic_position_component != nullptr) {
		set_position(mimic_position_component->get_position());
	}
	if (mimic_rotation_component != nullptr) {
		set_rotation(mimic_rotation_component->get_rotation());
	}
	if (mimic_size_component != nullptr) {
		set_size(mimic_size_component->get_size());
	}

	tick(dt);
}

void Object::set_position(const FVector3 &position) noexcept {
	transform.position = position;

	for (const std::unique_ptr<Object>& child : children) {
		child->set_position(position);
	}

	for (const std::unique_ptr<Component>& component : components) {
		component->set_position(position);
	}
}

void Object::set_rotation(const FVector3 &rotation) noexcept {
	transform.rotation = rotation;

	for (const std::unique_ptr<Object>& child : children) {
		child->set_position(rotation);
	}

	for (const std::unique_ptr<Component>& component : components) {
		component->set_rotation(rotation);
	}
}

void Object::set_size(const FVector3 &size) noexcept {
	transform.size = size;

	for (const std::unique_ptr<Object>& child : children) {
		child->set_position(size);
	}

	for (const std::unique_ptr<Component>& component : components) {
		component->set_size(size);
	}
}

void Object::translate(const FVector3 &position) noexcept {
	set_position(position + transform.position);
}

void Object::rotate(const FVector3 &rotation) noexcept {
	set_rotation(rotation + transform.rotation);
}

void Object::size(const FVector3 &size) noexcept {
	set_size(size + transform.size);
}

GET FVector3 Object::get_position() const noexcept {
	return transform.position;
}

GET FVector3 Object::get_rotation() const noexcept {
	return transform.rotation;
}

GET FVector3 Object::get_size() const noexcept {
	return transform.size;
}

void Object::set_transform(const Transform &transform) noexcept {
	set_position(transform.position);
	set_rotation(transform.rotation);
	set_size(transform.size);
}

GET Transform Object::get_transform() const noexcept {
	return transform;
}

bool Object::operator==(const Object &object) const noexcept {
	return (
		transform == object.transform &&
		name == object.name &&
		parent == object.parent &&
		children == object.children
	);
}

bool Object::operator!=(const Object &object) const noexcept {
	return !(*this == object);
}

Object* Object::get_parent() noexcept {
	return parent;
}

void Object::set_parent(Object* parent) noexcept {
	if (parent == this->parent) return;
	if (std::ranges::find_if(children, [&](const auto& obj){
		return obj.get() == parent;
	}) != children.end()) return; // TODO: Check whole tree

	std::set<std::unique_ptr<Object>>* container{&scene->objects};
	if (this->parent != nullptr) container = &this->parent->children;

	auto it{container->extract(
		std::ranges::find_if(*container, [&](const auto& obj){
			return obj.get() == this;
		})
	)};
	if (parent != nullptr) {
		parent->children.insert(std::move(it));
	} else {
		scene->objects.insert(std::move(it));
	}

	this->parent = parent;
}

Object* Object::add_object(std::unique_ptr<Object>&& obj) {
	obj->parent = this;
	if (scene != nullptr)
		obj->set_scene(scene);
	return children.insert(std::move(obj)).first->get();
}

Component* Object::add_component(std::unique_ptr<Component>&& component) {
	if (dynamic_cast<GraphicsComponent*>(component.get())) {
		if (scene != nullptr)
			scene->graphics_scene->add_component<GraphicsComponent>(static_cast<GraphicsComponent*>(component.get()));
	}

	if (scene != nullptr)
		component->set_scene(scene);

	return components.insert(std::move(component)).first->get();
}

std::set<Object*> Object::get_children() noexcept {
	return children | std::views::transform([&](const auto& obj){ return obj.get(); }) | std::ranges::to<std::set>();
}

void Object::set_scene(Scene* scene) noexcept {
	for (auto& obj : children) {
		obj->set_scene(scene);
	}
	this->scene = scene;
}

void Object::base_render_editor_gui_section(std::vector<ObjectsTreeNode>& nodes) {
	nodes.reserve(nodes.capacity() + children.size());

	ObjectsTreeNode last_node{};
	if (!nodes.empty())	last_node = nodes.back();
	else				last_node = ObjectsTreeNode{nullptr, 1u+nodes.size(), 0u};
	
	if (children.empty())
		nodes.push_back(ObjectsTreeNode{this, 1u+nodes.size(), 0});
	else
		nodes.push_back(ObjectsTreeNode{this, children.size()+last_node.child_index, children.size()});
}

void Object::base_render_editor_gui_section() {
	ImGui::Begin("Inspector");

	// TODO: Displayed name will be incorrect when name is programmatically changed
	static std::string in_name = name;
	if (ImGui::InputText("Name", &in_name, ImGuiInputTextFlags_EnterReturnsTrue)) {
		name = in_name;
	}

	ImGui::Text("Transform");
	float vec[3]{transform.position.x, transform.position.y, transform.position.z};
	if (ImGui::InputFloat3("Position", vec))
		set_position(vec);
	vec[0] = transform.rotation.x;
	vec[1] = transform.rotation.y;
	vec[2] = transform.rotation.z;
	if (ImGui::InputFloat3("Rotation", vec))
		set_rotation(vec);
	vec[0] = transform.size.x;
	vec[1] = transform.size.y;
	vec[2] = transform.size.z;
	if (ImGui::InputFloat3("Size", vec))
		set_size(vec);

	ImGui::Text("Components");
	int i = 0;
	for (const std::unique_ptr<Component>& component : components) {
		if (ImGui::TreeNode(("Component " + std::to_string(i)).c_str())) {
			component->base_render_editor_gui_section();
			ImGui::TreePop();
		}
		i++;
	}

	/*ImGui::Text("Children");
	i = 0;
	for (auto& child : children) {
		std::string name{child->name};
		if (child->name.empty()) {
			i++;
			name = "Unnamed object ";
			name += std::to_string(i);
		}
		if (ImGui::TreeNode(name.c_str())) {
			child->base_render_editor_gui_section();
			ImGui::TreePop();
		}
	}*/

	render_editor_gui_section();

	ImGui::End();
}
