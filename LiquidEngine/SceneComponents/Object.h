#pragma once

#include <DirectXMath.h>
#include <concepts>
#include "../Utility/globalstructs.h"
#include "../Mesh.h"
#include "../Components/Component.h"
#include "../Controllable.h"

using namespace DirectX;

class Scene;

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

	void add_component(const std::shared_ptr<Component>& component, Component* parent = nullptr); //? AHHHHHHHHHHHHHHHHHHHHHHHH

	GET std::set<std::weak_ptr<Object>, std::owner_less<std::weak_ptr<Object>>> get_children() noexcept;

	//ReadObjFileDataOutput read_obj_file(const std::vector<std::string> &content, const ReadObjFileDataOutput &mesh_out) noexcept;
	
	bool operator==(const Object &object) const noexcept;
	bool operator!=(const Object &object) const noexcept;

	void base_render_editor_gui_section() override;

	std::string name{""};

	Component* mimic_position_component{nullptr};
	Component* mimic_rotation_component{nullptr};
	Component* mimic_size_component{nullptr};

private:
	friend class Scene;

	Transform transform{};

	Object* parent{nullptr};
	std::unordered_set<std::shared_ptr<Object>> children{};

	Scene* scene{nullptr};

	void remove_this_from_parents_children() {
		if (auto it{std::ranges::find_if(parent->children, [&](const std::shared_ptr<Object>& c){
			return c.get() == this;
		})}; it != children.end()) {
			parent->children.erase(it);
		}
	}
};
