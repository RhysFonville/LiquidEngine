#pragma once

#include <set>
#include <ranges>
#include <concepts>
#include "../Utility/ObjectStructs.h"
#include "../Controllable.h"
#include "../imgui/imgui.h"

class Component;

/**
 * Class shared between object and component classes. Gives them a list of components and functions for them.
 */
class ComponentHolder {
public: 
	template <typename T>
	requires std::derived_from<T, Component>
	GET T* get_component() noexcept {
		for (const std::unique_ptr<Component>& component : components) {
			if (T* ret{dynamic_cast<T*>(component.get())}) {
				return ret;
			}
		}
		return nullptr;
	}

	template <typename T>
	requires std::derived_from<T, Component>
	GET std::set<T*> get_components() noexcept {
		std::set<T*> ret_comps{};
		for (const std::unique_ptr<Component>& component : components) {
			if (T* ret{dynamic_cast<T*>(component.get())}) {
				ret_comps.insert(ret);
			}
		}
		return ret_comps;
	}

	GET std::set<Component*> get_all_components() noexcept {
		return components | std::views::transform([&](const std::unique_ptr<Component>& sp) {
			return sp.get();
		}) | std::ranges::to<std::set>();
	}

protected:
	std::set<std::unique_ptr<Component>> components{}; //! HAS TO BE POINTER SO WE CAN CAST TO SUBCLASSES
};

class GraphicsComponent;

class Object;
class Scene;

/**
* Base component class for all components.
*/
class Component : public Controllable, public ComponentHolder {
public:
	Component() : ComponentHolder{} { }

	void base_clean_up() override;

	void clean_components();

	virtual void set_position(FVector3 position) noexcept;
	virtual void set_rotation(FVector3 rotation) noexcept;
	virtual void set_size(FVector3 size) noexcept;

	virtual void translate(FVector3 position) noexcept;
	virtual void rotate(FVector3 rotation) noexcept;
	virtual void size(FVector3 size) noexcept;

	virtual GET FVector3 get_position() const noexcept;
	virtual GET FVector3 get_rotation() const noexcept;
	virtual GET FVector3 get_size() const noexcept;

	virtual void set_transform(Transform transform) noexcept;
	virtual GET Transform get_transform() const noexcept;

	GET Component* get_parent() const noexcept;
	void set_parent(Component* parent) noexcept;

	Component* add_component(std::unique_ptr<Component>&& component);

	void set_scene(Scene* scene) noexcept;

	virtual bool operator==(const Component &component) const noexcept;
	virtual void operator=(const Component &component) noexcept;

	void base_render_editor_gui_section() override;

protected:
	Transform transform;
	Component* parent{nullptr};
	Object* object{nullptr};
	Scene* scene{nullptr};
};
