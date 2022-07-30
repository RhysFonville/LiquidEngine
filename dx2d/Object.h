#pragma once

#include <DirectXMath.h>
#include "MeshComponent.h"
#include "Component.h"
#include "globalstructs.h"
#include "Storage.h"

using namespace DirectX;

class Object {
public:
	Object(std::string name = "New Object");

	void set_position(const FPosition3 &position) noexcept;
	void set_rotation(const FRotation3 &rotation) noexcept;
	void set_size(const FSize3 &size) noexcept;

	void translate(const FPosition3 &position) noexcept;
	void rotate(const FRotation3 &rotation) noexcept;
	void size(const FSize3 &size) noexcept;

	void set_transform(const Transform &transform) noexcept;
	GET Transform get_transform() const noexcept;

	bool has_component(const Component::Type &search) const noexcept;

	std::shared_ptr<Object> get_parent() noexcept;
	void set_parent(const std::shared_ptr<Object> &parent) noexcept;
	void remove_parent() noexcept;

	std::vector<std::shared_ptr<Object>> get_children() noexcept;
	void set_children(const std::vector<std::shared_ptr<Object>> &children) noexcept;

	void add_child(const std::shared_ptr<Object> &child) noexcept;

	ReadObjFileDataOutput read_obj_file(const std::vector<std::string> &content, const ReadObjFileDataOutput &mesh_out) noexcept;

	template <typename T>
	GET std::shared_ptr<T> get_component() const noexcept {
		for (std::shared_ptr<Component> component : components) {
			if (component->type == T::component_type) {
				return std::static_pointer_cast<T>(component);
			}
		}
		return nullptr;
	}

	template <typename T>
	GET std::vector<std::shared_ptr<T>> get_components() const noexcept {
		std::vector<std::shared_ptr<T>> ret;
		for (std::shared_ptr<Component> component : components) {
			if (component->type == T::component_type) {
				ret.push_back(std::static_pointer_cast<T>(component));
			}
		}
		return ret;
	}

	
	bool operator==(const Object &object) const noexcept;

	void clean_up();

	std::vector<std::shared_ptr<Component>> components;

	bool is_static = true;

	std::string name;

private:
	Transform transform;

	std::shared_ptr<Object> parent = nullptr;
	std::vector<std::shared_ptr<Object>> children;
};

