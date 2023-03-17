#pragma once

#include <DirectXMath.h>
#include "MeshComponent.h"
#include "Component.h"
#include "globalstructs.h"
#include "Storage.h"

using namespace DirectX;

class MechanicsData {
public:
	FVector3 previous_position = FVector3();

	FVector3 velocity = FVector3();
	FVector3 acceleration = FVector3();
	float speed = 0.0f; 
	float kinetic_energy = 0.0f;
	FVector3 momentum = FVector3();

	std::vector<Mechanics::Force> forces;

	GET FVector3 get_net_force() const noexcept {
		FVector3 net_force;

		for (const Mechanics::Force &force : forces) {
			net_force += force;
		}

		return net_force;
	}

	GET float get_mass() const noexcept {
		return mass;
	}

	void set_mass(float mass) {
		if (mass != 0) {
			this->mass = mass;
		} else {
			throw std::exception("An object's mass must be set to >0.0f kilograms.");
		}
	}

private:
	float mass = 20.0f;
};

class Object {
public:
	Object(std::string name = "New Object");
	~Object();

	void set_position(const FVector3 &position) noexcept;
	void set_rotation(const FVector3 &rotation) noexcept;
	void set_size(const FVector3 &size) noexcept;

	void translate(const FVector3 &position) noexcept;
	void rotate(const FVector3 &rotation) noexcept;
	void size(const FVector3 &size) noexcept;

	void set_transform(const Transform &transform) noexcept;
	GET Transform get_transform() const noexcept;

	GET bool has_component(const Component::Type &search) const noexcept;
	template <typename T>
	GET bool has_component() const {
		for (const std::shared_ptr<Component> &component : components) {
			if (component->get_type() == T::component_type) {
				return true;
			}
		}
		return false;
	}

	GET std::shared_ptr<Object> get_parent() noexcept;
	void set_parent(const std::shared_ptr<Object> &parent) noexcept;
	void remove_parent() noexcept;

	GET std::vector<std::shared_ptr<Object>> get_children() noexcept;
	void set_children(const std::vector<std::shared_ptr<Object>> &children) noexcept;

	void add_child(const std::shared_ptr<Object> &child) noexcept;

	void compile();

	ReadObjFileDataOutput read_obj_file(const std::vector<std::string> &content, const ReadObjFileDataOutput &mesh_out) noexcept;

	template <typename T, ACCEPT_BASE_AND_HEIRS_ONLY(Component)>
	GET std::shared_ptr<T> get_component() noexcept {
		for (const std::shared_ptr<Component> &component : components) {
			if (component->get_type() == T::component_type) {
				return std::static_pointer_cast<T>(component);
			}
		}
		return nullptr;
	}

	template <typename T, ACCEPT_BASE_AND_HEIRS_ONLY(Component)>
	GET std::vector<std::shared_ptr<T>> get_components() noexcept {
		std::vector<std::shared_ptr<T>> ret;
		for (const std::shared_ptr<Component> &component : components) {
			if (component->get_type() == T::component_type) {
				ret.push_back(std::static_pointer_cast<T>(component));
			}
		}
		return ret;
	}

	template <typename T, ACCEPT_BASE_AND_HEIRS_ONLY(Component)>
	void add_component(std::shared_ptr<T> component) {
		components_added.push_back(std::make_pair(component, components.size()));
		components.push_back(component);
	}

	void remove_component(size_t index) {
		components_removed.push_back(std::make_pair(
			components[index], 
			index
		));
		components.erase(components.begin()+index);
	}

	void clear_component_history() noexcept;

	const std::vector<std::pair<std::shared_ptr<Component>, size_t>> & get_added_components() const noexcept;
	const std::vector<std::pair<std::shared_ptr<Component>, size_t>> & get_removed_components() const noexcept;
	
	bool operator==(const Object &object) const noexcept;
	bool operator!=(const Object &object) const noexcept;

	void clean_up();

	bool is_static = true;

	std::string name;

	MechanicsData mechanics;

private:
	Transform transform;
	
	std::vector<std::shared_ptr<Component>> components;
	std::vector<std::pair<std::shared_ptr<Component>, size_t>> components_added;
	std::vector<std::pair<std::shared_ptr<Component>, size_t>> components_removed;

	std::shared_ptr<Object> parent = nullptr;
	std::vector<std::shared_ptr<Object>> children;
};
