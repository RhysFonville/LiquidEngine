#include "Object.h"
#include "Scene.h"

void Object::clean_components() {
	for (const std::shared_ptr<Component>& component : components) {
		component->base_clean_up();
	}
	components.clear();
}

void Object::base_clean_up() {
	remove_this_from_parents_children();
	children.clear();
	clean_components();
	clean_up();
}

void Object::base_compile() {
	for (const std::shared_ptr<Object>& child : children) {
		child->compile();
	}

	for (const std::shared_ptr<Component>& comp : components) {
		comp->base_compile();
	}

	compile();
}

void Object::base_tick(float dt) {
	for (const std::shared_ptr<Object>& child : children) {
		child->base_tick(dt);
	}

	for (const std::shared_ptr<Component>& comp : components) {
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

	for (const std::shared_ptr<Object>& child : children) {
		child->set_position(position);
	}

	for (const std::shared_ptr<Component>& component : components) {
		component->set_position(position);
	}
}

void Object::set_rotation(const FVector3 &rotation) noexcept {
	transform.rotation = rotation;

	for (const std::shared_ptr<Object>& child : children) {
		child->set_position(rotation);
	}

	for (const std::shared_ptr<Component>& component : components) {
		component->set_rotation(rotation);
	}
}

void Object::set_size(const FVector3 &size) noexcept {
	transform.size = size;

	for (const std::shared_ptr<Object>& child : children) {
		child->set_position(size);
	}

	for (const std::shared_ptr<Component>& component : components) {
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
	if (parent == nullptr) return;
	parent->children.insert(std::make_shared<Object>(*this));
	remove_this_from_parents_children();
	this->parent = parent;
}

void Object::add_component(const std::shared_ptr<Component>& component, Component* parent) {
	component->set_parent(parent);
	components.insert(component);

	//if (GraphicsComponent::is_graphics_component(*components.back())) {
	if (std::dynamic_pointer_cast<GraphicsComponent>(component)) {
		scene->graphics_scene->add_component<GraphicsComponent>(std::static_pointer_cast<GraphicsComponent>(component).get());
	}

	/*if (std::dynamic_pointer_cast<PhysicalComponent>(component)) {
		//physics_scene->objects.push_back(std::static_pointer_cast<PhysicalComponent>(components.back()).get());
	}*/
}

std::set<std::weak_ptr<Object>, std::owner_less<std::weak_ptr<Object>>> Object::get_children() noexcept {
	return children | std::views::transform([&](const std::shared_ptr<Object>& sp) {
		return std::weak_ptr<Object>{sp};
	}) | std::ranges::to<std::set<std::weak_ptr<Object>, std::owner_less<std::weak_ptr<Object>>>>();
}

void Object::base_render_editor_gui_section() {
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
	for (const std::shared_ptr<Component>& component : components) {
		if (ImGui::TreeNode(("Component " + std::to_string(i)).c_str())) {
			component->base_render_editor_gui_section();
			ImGui::TreePop();
		}
		i++;
	}

	render_editor_gui_section();
}
