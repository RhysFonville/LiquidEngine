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

	GET Object* get_parent() noexcept;
	void set_parent(const Object* parent) noexcept;
	void remove_parent() noexcept;

	GET std::vector<Object> & get_children() noexcept;
	void set_children(const std::vector<Object*> &children) noexcept;

	void add_child(const Object* child) noexcept;

	void compile();

	ReadObjFileDataOutput read_obj_file(const std::vector<std::string> &content, const ReadObjFileDataOutput &mesh_out) noexcept;

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
	void add_component(T component) { //? AHHHHHHHHHHHHHHHHHHHHHHHH
		components.push_back(std::make_unique<T>(component));
	}

	void remove_component(size_t index) {
		components.erase(components.begin()+index);
	}
	
	bool operator==(const Object &object) const noexcept;
	bool operator!=(const Object &object) const noexcept;

	void clean_up();

	bool is_static = true;

	std::string name;

	MechanicsData mechanics;

private:
	Transform transform;
	
	std::vector<std::shared_ptr<Component>> components; //! HAS TO BE POINTER SO WE CAN CAST TO SUBCLASSES

	Object* parent = nullptr;
	std::vector<Object*> children;
};
