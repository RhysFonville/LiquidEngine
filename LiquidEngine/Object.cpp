#include "Object.h"

Object::~Object() {
	clean_up();
}

void Object::set_position(const FVector3 &position) noexcept {
	transform.position = position;

	for (Object* child : children) {
		child->set_position(position);
	}

	for (std::shared_ptr<Component> &component : components) {
		component->set_position(position);
	}
}

void Object::set_rotation(const FVector3 &rotation) noexcept {
	transform.rotation = rotation;

	for (Object* child : children) {
		child->set_position(rotation);
	}

	for (std::shared_ptr<Component> &component : components) {
		component->set_rotation(rotation);
	}
}

void Object::set_size(const FVector3 &size) noexcept {
	transform.size = size;

	for (Object* child : children) {
		child->set_position(size);
	}

	for (std::shared_ptr<Component> &component : components) {
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

void Object::base_clean_up() {
	remove_this_from_parents_children();

	for (Object* child : children) {
		child->clean_up();
		child->parent = nullptr;
	}

	root_component.base_clean_up();
	for (const std::shared_ptr<Component> &comp : components) {
		comp->base_compile();
	}

	clean_up();
}

void Object::base_compile() {
	for (Object* child : children) {
		child->compile();
	}

	root_component.base_compile();
	for (const std::shared_ptr<Component> &comp : components) {
		comp->base_compile();
	}

	compile();
}

void Object::base_tick(float dt) {
	for (Object* child : children) {
		child->base_tick(dt);
	}

	root_component.base_tick(dt);
	for (const std::shared_ptr<Component> &comp : components) {
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

//ReadObjFileDataOutput Object::read_obj_file(const std::vector<std::string> &content, const ReadObjFileDataOutput &mesh_out) noexcept {
//	StaticMeshComponent mc;
//	ReadObjFileDataOutput out = mc.read_obj_file(ReadObjFileDataInput(content, mesh_out));
//
//	root_component.add_component(mc);
//
//	return out;
//}

bool Object::has_component(Component::Type search) const noexcept {
	for (const std::shared_ptr<Component> &component : components) {
		if (component->get_type() == search) {
			return true;
		}
	}
	return false;
}

//std::shared_ptr<Component> add_component(Component::Type type) {
//	int count = 0;
//	auto comp_if = [&]<typename T>(Component::Type comp_type) {
//		if (comp_type == type) {
//			auto sp = std::make_shared<T>();
//			add_component(sp);
//			count++;
//			return sp;
//		} else {
//			return nullptr;
//		}
//	};
//	
//	if (auto comp = comp_if<StaticMeshComponent>(Component::Type::StaticMeshComponent);
//		comp != nullptr) { }
//
//	if (count < (int)Component::Type::EnumEnd) { }
//}

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
	for (std::shared_ptr<Component> &component : get_all_components()) {
		if (ImGui::TreeNode(("Component " + std::to_string(i)).c_str())) {
			component->base_render_editor_gui_section();
			ImGui::TreePop();
		}
		i++;
	}

	render_editor_gui_section();
}
