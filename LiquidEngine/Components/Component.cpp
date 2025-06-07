#include "Component.h"

void Component::base_clean_up() {
	clean_components();
	clean_up();
}

void Component::clean_components() {
	for (const std::shared_ptr<Component>& component : components) {
		component->base_clean_up();
	}
	components.clear();
}

void Component::set_position(FVector3 position) noexcept {
	transform.position = position;
}

void Component::set_rotation(FVector3 rotation) noexcept {
	transform.rotation = rotation;
}

void Component::set_size(FVector3 size) noexcept {
	transform.size = size;
}

void Component::translate(FVector3 position) noexcept {
	set_position(position + transform.position);
}

void Component::rotate(FVector3 rotation) noexcept {
	set_rotation(rotation + transform.rotation);
}

void Component::size(FVector3 size) noexcept {
	set_size(size + transform.size);
}

FVector3 Component::get_position() const noexcept {
	return transform.position;
}

FVector3 Component::get_rotation() const noexcept {
	return transform.rotation;
}

FVector3 Component::get_size() const noexcept {
	return transform.size;
}

void Component::set_transform(Transform transform) noexcept {
	set_position(transform.position);
	set_rotation(transform.rotation);
	set_size(transform.size);
}

Transform Component::get_transform() const noexcept {
	return transform;
}

Component* Component::get_parent() const noexcept {
	return parent;
}

void Component::set_parent(Component* parent) noexcept {
	parent->components.insert(std::make_shared<Component>(this));
	remove_this_from_parents_children();
	this->parent = parent;
}

bool Component::operator==(const Component &component) const noexcept {
	return (component.transform == transform && component.components == components);
}

void Component::operator=(const Component &component) noexcept {
	transform = component.transform;
	components = component.components;
}

void Component::base_render_editor_gui_section() {
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

	render_editor_gui_section();
}
