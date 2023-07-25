#pragma once

#include <DirectXMath.h>
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

	void clean_up();
	void compile();
	void tick();

	ReadObjFileDataOutput read_obj_file(const std::vector<std::string> &content, const ReadObjFileDataOutput &mesh_out) noexcept;
	
	bool operator==(const Object &object) const noexcept;
	bool operator!=(const Object &object) const noexcept;

	std::string name;

	MechanicsData mechanics;

	Component root_component;

private:
	Transform transform;

	Object* parent = nullptr;
	std::vector<Object*> children;

	void remove_this_from_parents_children() {
		if (parent != nullptr) {
			this->parent->children.erase(std::remove(
				this->parent->children.begin(),
				this->parent->children.end(),
				this),
				this->parent->children.end()
			);
		}
	}
};
