#include "Object.h"

Object::Object(std::string name) : is_static(true), name(name) {}

void Object::set_position(const FPosition3 &position) noexcept {
	transform.position = position;

	for (std::shared_ptr<Component> &component : components) {
		component->set_position(position);
	}

	for (const std::shared_ptr<Object> &child : children) {
		child->set_position(position);
	}
}

void Object::set_rotation(const FRotation3 &rotation) noexcept {
	transform.rotation = rotation;

	for (std::shared_ptr<Component> &component : components) {
		component->set_rotation(rotation);
	}

	for (const std::shared_ptr<Object> &child : children) {
		child->set_position(rotation);
	}
}

void Object::set_size(const FSize3 &size) noexcept {
	transform.size = size;

	for (std::shared_ptr<Component> &component : components) {
		component->set_size(size);
	}

	for (const std::shared_ptr<Object> &child : children) {
		child->set_position(size);
	}
}

void Object::translate(const FPosition3 &position) noexcept {
	set_position(position + transform.position);
}

void Object::rotate(const FRotation3 &rotation) noexcept {
	set_rotation(rotation + transform.rotation);
}

void Object::size(const FSize3 &size) noexcept {
	set_size(size + transform.size);
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
	return (components == object.components &&
		transform == object.transform);
}

bool Object::has_component(const Component::Type &search) const noexcept {
	for (const std::shared_ptr<Component> &component : components) {
		if (component->type == search) {
			return true;
		}
	}
	return false;
}

void Object::clean_up() {
	for (std::shared_ptr<Component> &component : components) {
		component->clean_up();
	}
}

std::shared_ptr<Object> Object::get_parent() noexcept {
	return parent;
}

//void Object::set_parent(const std::shared_ptr<Object> &parent) noexcept {
//	this->parent->children.erase(std::remove(
//		this->parent->children.begin(),
//		this->parent->children.end(),
//		*this),
//		this->parent->children.end());
//	parent->children.push_back(std::make_shared<Object>(*this));
//
//	this->parent = parent;
//}
//
//void Object::remove_parent() noexcept {
//	this->parent->children.erase(std::remove(
//		this->parent->children.begin(),
//		this->parent->children.end(),
//		*this),
//		this->parent->children.end());
//
//	parent = nullptr;
//}
//
//std::vector<std::shared_ptr<Object>> Object::get_children() noexcept {
//	return children;
//}
//
//void Object::set_children(const std::vector<std::shared_ptr<Object>> &children) noexcept {
//	for (const std::shared_ptr<Object> &child : children) {
//		child->set_parent(std::make_shared<Object>(*this));
//	}
//}
//
//void Object::add_child(const std::shared_ptr<Object> &child) noexcept {
//	child->set_parent(std::make_shared<Object>(*this));
//}

ReadObjFileDataOutput Object::read_obj_file(const std::vector<std::string> &content, const ReadObjFileDataOutput &mesh_out) noexcept {
	MeshComponent mc;

	ReadObjFileDataOutput out = mc.read_obj_file(ReadObjFileDataInput(content, mesh_out));

	std::string line;
	for (const std::string line : content) {
		if (line.substr(0, 6) == "usemtl") {
			mc.material = Storage::get_material_by_name(line.substr(7));
		}
	}

	components.push_back(std::make_shared<MeshComponent>(mc));

	return out;
}
