#pragma once

#include <DirectXMath.h>
#include "Utility/globalstructs.h"
#include "Mesh.h"
#include "Components/Component.h"
#include "Graphics/GraphicsScene.h"
#include "Physics/PhysicsScene.h"
#include "Controllable.h"

using namespace DirectX;

/**
 * Base class for all objects.
 */
class Object : public Controllable, public ComponentHolder {
public:
	Object() : ComponentHolder{} { }
	
	void clean_components();

	void base_clean_up() override;
	void base_compile() override;
	void base_tick(float dt) override;

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

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	void add_component(const std::shared_ptr<T>& component) { //? AHHHHHHHHHHHHHHHHHHHHHHHH
		component->set_parent(nullptr);
		components.insert(component);

		//if (GraphicsComponent::is_graphics_component(*components.back())) {
		/*if (std::dynamic_pointer_cast<GraphicsComponent>(component)) {
			graphics_scene->add_component<GraphicsComponent>(std::static_pointer_cast<GraphicsComponent>(components.back()).get());
		}

		if (std::dynamic_pointer_cast<PhysicalComponent>(component)) {
			physics_scene->objects.push_back(std::static_pointer_cast<PhysicalComponent>(components.back()).get());
		}*/
	}

	GET std::set<std::weak_ptr<Object>, std::owner_less<std::weak_ptr<Object>>> get_children() noexcept;

	//ReadObjFileDataOutput read_obj_file(const std::vector<std::string> &content, const ReadObjFileDataOutput &mesh_out) noexcept;
	
	bool operator==(const Object &object) const noexcept;
	bool operator!=(const Object &object) const noexcept;

	void base_render_editor_gui_section() override;

	std::string name{""};

	Component* mimic_position_component{nullptr};
	Component* mimic_rotation_component{nullptr};
	Component* mimic_size_component{nullptr};

	GraphicsScene* graphics_scene{nullptr};
	PhysicsScene* physics_scene{nullptr};

private:
	Transform transform{};

	Object* parent{};
	std::unordered_set<std::shared_ptr<Object>> children{};

	void remove_this_from_parents_children() {
		if (auto it{std::ranges::find_if(parent->children, [&](const std::shared_ptr<Object>& c){
			return c.get() == this;
		})}; it != children.end()) {
			parent->children.erase(it);
		}
	}
};
