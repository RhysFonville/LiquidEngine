#pragma once

#include <DirectXMath.h>
#include <concepts>
#include "../Utility/globalstructs.h"
#include "../Mesh.h"
#include "../Components/Component.h"
#include "../Controllable.h"

using namespace DirectX;

struct ObjectsTreeNode;
class Scene;

/**
 * Base class for all objects.
 */
class Object : public Controllable, public ComponentHolder {
public:
	Object() : ComponentHolder{} { }
	Object(const Object&) = delete;

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

	Object* add_object(std::unique_ptr<Object>&& obj);
	
	Component* add_component(std::unique_ptr<Component>&& component); //? AHHHHHHHHHHHHHHHHHHHHHHHH
	void remove_component(Component* component);

	GET std::set<Object*> get_children() const noexcept;
	GET size_t get_children_count() const noexcept { return children.size(); }

	void set_scene(Scene* scene) noexcept;

	bool operator==(const Object &object) const noexcept;
	bool operator!=(const Object &object) const noexcept;

	void base_render_editor_gui_section_tree(Object** selected_object);
	void base_render_editor_gui_section() override;

	std::string name{""};

	Component* mimic_position_component{nullptr};
	Component* mimic_rotation_component{nullptr};
	Component* mimic_size_component{nullptr};

protected:
	friend class Component;
	Scene* scene{nullptr};

private:
	Transform transform{};

	Object* parent{nullptr};
	std::set<std::unique_ptr<Object>> children{};
};

static Object* selected_object{nullptr};
