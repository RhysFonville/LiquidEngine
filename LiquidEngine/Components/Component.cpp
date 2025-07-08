#include "Component.h"
#include "../Core/Object.h"
#include "GraphicsComponent.h"
#include "../Core/Scene.h"

void Component::base_clean_up() {
	clean_components();
	clean_up();
}

void Component::clean_components() {
	for (const std::unique_ptr<Component>& component : components) {
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
	if (parent == this->parent) return;
	if (std::ranges::find_if(components, [&](const auto& obj) {
		return obj.get() == parent;
		}) != components.end()) return;

	std::set<std::unique_ptr<Component>>* container{&object->components};
	if (this->parent != nullptr) container = &this->parent->components;

	auto it{container->extract(
		std::ranges::find_if(*container, [&](const auto& obj) {
			return obj.get() == this;
			})
	)};
	if (parent != nullptr) {
		parent->components.insert(std::move(it));
	} else {
		object->components.insert(std::move(it));
	}

	this->parent = parent;
}

Component* Component::add_component(std::unique_ptr<Component>&& component) {
	if (dynamic_cast<GraphicsComponent*>(component.get())) {
		scene->graphics_scene->add_component<GraphicsComponent>(static_cast<GraphicsComponent*>(component.get()));
	}
	/*if (std::dynamic_pointer_cast<PhysicalComponent>(component)) {
		//physics_scene->objects.push_back(std::static_pointer_cast<PhysicalComponent>(components.back()).get());
	}*/

	if (scene != nullptr)
		component->set_scene(scene);

	component->parent = this;

	return components.insert(std::move(component)).first->get();
}

void Component::set_scene(Scene* scene) noexcept {
	for (auto& obj : components) {
		obj->set_scene(scene);
	}
	this->scene = scene;
}

bool Component::operator==(const Component &component) const noexcept {
	return (component.transform == transform && component.components == components);
}

void Component::operator=(const Component &component) noexcept {
	transform = component.transform;
	// TODO
	//components = component.components;
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
