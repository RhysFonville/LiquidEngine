#pragma once

#include <DirectXMath.h>
#include "globalstructs.h"
#include "Mesh.h"
#include "Components/Component.h"

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

	void clean_up();
	void compile();
	void base_tick(float dt);
	virtual void tick(float dt) { }

	//ReadObjFileDataOutput read_obj_file(const std::vector<std::string> &content, const ReadObjFileDataOutput &mesh_out) noexcept;
	
	bool operator==(const Object &object) const noexcept;
	bool operator!=(const Object &object) const noexcept;

	std::string name;

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
