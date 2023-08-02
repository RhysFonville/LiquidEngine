#include "Component.h"

Component::Component(const Type &type, const Transform &transform)
	: type(type), transform(transform) { }

void Component::set_position(FVector3 position) noexcept {
	transform.position = position;

	for (std::shared_ptr<Component> &component : components) {
		component->set_position(position);
	}
}

void Component::set_rotation(FVector3 rotation) noexcept {
	transform.rotation = rotation;

	for (std::shared_ptr<Component> &component : components) {
		component->set_rotation(rotation);
	}
}

void Component::set_size(FVector3 size) noexcept {
	transform.size = size;

	for (std::shared_ptr<Component> &component : components) {
		component->set_size(size);
	}
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

Component::Type Component::get_type() const noexcept {
	return type;
}

bool Component::operator==(const Component &component) const noexcept {
	return (component.type == type && component.transform == transform);
}

void Component::operator=(const Component &component) noexcept {
	transform = component.transform;
	type = component.type;
}

bool Component::has_component(Component::Type search) const noexcept {
	for (const std::shared_ptr<Component> &component : components) {
		if (component->get_type() == search) {
			return true;
		}
	}
	return false;
}