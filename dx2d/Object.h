#pragma once

#include <DirectXMath.h>
#include "globalstructs.h"
#include "Mesh.h"
#include "Components/Component.h"
#include "Graphics/GraphicsScene.h"

using namespace DirectX;

class Object {
public:
	Object() { }
	~Object();

	void set_position(const FVector3 &position) noexcept;
	void set_rotation(const FVector3 &rotation) noexcept;
	void set_size(const FVector3 &size) noexcept;

	void translate(const FVector3 &position) noexcept;
	void rotate(const FVector3 &rotation) noexcept;
	void size(const FVector3 &size) noexcept;

	GET FVector3 get_position() const noexcept;
	GET FVector3 get_rotation() const noexcept;
	GET FVector3 get_size() const noexcept;

	void set_transform(const Transform &transform) noexcept;
	GET Transform get_transform() const noexcept;

	GET Object* get_parent() noexcept;
	void set_parent(Object* parent) noexcept;
	void remove_parent() noexcept;

	GET std::vector<Object*> & get_children() noexcept;

	void add_child(Object* child) noexcept;

	void base_clean_up();
	void base_compile();
	void base_tick(float dt);

	virtual void on_start() { }
	virtual void clean_up() { }
	virtual void compile() { }
	virtual void tick(float dt) { }

	//ReadObjFileDataOutput read_obj_file(const std::vector<std::string> &content, const ReadObjFileDataOutput &mesh_out) noexcept;
	
	bool operator==(const Object &object) const noexcept;
	bool operator!=(const Object &object) const noexcept;

	GET bool has_component(Component::Type search) const noexcept;
	template <typename T>
	GET bool has_component() const {
		for (const std::shared_ptr<Component> &component : components) {
			if (component->get_type() == T::component_type) {
				return true;
			}
		}
		return false;
	}

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	GET T* get_component() noexcept {
		for (std::shared_ptr<Component> &component : components) {
			if (component->get_type() == T::component_type) {
				return (T*)component.get();
			}
		}
		return nullptr;
	}

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	GET std::vector<T*> get_components() noexcept {
		std::vector<T*> ret;
		for (std::shared_ptr<Component> &component : components) {
			if (component->get_type() == T::component_type) {
				ret.push_back((T*)component.get());
			}
		}
		return ret;
	}

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	void add_component(const std::shared_ptr<T> &component) { //? AHHHHHHHHHHHHHHHHHHHHHHHH
		components.push_back(component);

		if (GraphicsComponent::is_graphics_component(*components.back())) {
			graphics_scene->add_component<GraphicsComponent>(std::static_pointer_cast<GraphicsComponent>(components.back()).get());
		}

		components.back()->parent = &root_component;
	}

	void remove_component(size_t index) {
		components.erase(components.begin()+index);

		if (GraphicsComponent::is_graphics_component(*components.back())) {
			graphics_scene->remove_component(std::static_pointer_cast<GraphicsComponent>(*(components.begin()+index)).get());
		}
	}

	std::string name = "";

	Component root_component;

	GraphicsScene* graphics_scene = nullptr;

private:
	Transform transform;

	Object* parent = nullptr;
	std::vector<Object*> children = { };

	std::vector<std::shared_ptr<Component>> components = { }; //! HAS TO BE POINTER SO WE CAN CAST TO SUBCLASSES

	void remove_this_from_parents_children() {
		if (parent != nullptr) {
			this->parent->children.erase(
				std::remove(
					this->parent->children.begin(),
					this->parent->children.end(),
				this),
				this->parent->children.end()
			);
		}
	}
};
