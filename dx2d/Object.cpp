#include "Object.h"

Object::Object(std::string name) : name(name) {}

Object::~Object() {
	clean_up();
}

void Object::set_position(const FVector3 &position) noexcept {
	mechanics.previous_position = transform.position;
	transform.position = position;

	for (Object* child : children) {
		child->set_position(position);
	}
}

void Object::set_rotation(const FVector3 &rotation) noexcept {
	transform.rotation = rotation;

	for (Object* child : children) {
		child->set_position(rotation);
	}
}

void Object::set_size(const FVector3 &size) noexcept {
	transform.size = size;

	for (Object* child : children) {
		child->set_position(size);
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
	return (root_component == object.root_component &&
		transform == object.transform &&
		name == object.name &&
		parent == object.parent &&
		children == object.children);
}

bool Object::operator!=(const Object &object) const noexcept {
	return (root_component != object.root_component ||
		transform != object.transform ||
		name != object.name ||
		parent != object.parent ||
		children != object.children);
}

void Object::clean_up() {
	remove_this_from_parents_children();

	for (Object* child : children) {
		child->parent = nullptr;
	}

	root_component.clean_up();
}

void Object::compile() {
	root_component.compile();
}

void Object::tick() {
	root_component.tick();
}

Object* Object::get_parent() noexcept {
	return parent;
}

void Object::set_parent(Object* parent) noexcept {
	remove_this_from_parents_children();
	parent->add_child(this);

	this->parent = parent;
}

void Object::remove_parent() noexcept {
	parent = nullptr;
}

std::vector<Object*> & Object::get_children() noexcept {
	return children;
}

void Object::add_child(Object* child) noexcept {
	 children.push_back(child);
	 child->remove_this_from_parents_children();
	 child->parent = this;
}

void Object::compile() {
	for (std::shared_ptr<Component> &component : components) {
		component->compile();
	}
}

ReadObjFileDataOutput Object::read_obj_file(const std::vector<std::string> &content, const ReadObjFileDataOutput &mesh_out) noexcept {
	MeshComponent mc;

	ReadObjFileDataOutput out = mc.read_obj_file(ReadObjFileDataInput(content, mesh_out));

	/*std::string line;
	for (const std::string line : content) {
		if (line.substr(0, 6) == "usemtl") {
			mc.material = Storage::get_material_by_name(line.substr(7));
		}
	}*/

	add_component(mc);

	return out;
}
