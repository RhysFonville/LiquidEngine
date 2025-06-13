#pragma once

#include <unordered_set>
#include <set>
#include <ranges>
#include "../Utility/ObjectStructs.h"
#include "../Controllable.h"
#include "../imgui/imgui.h"

class Component;

/**
 * Class shared between object and component classes. Gives them a list of components and functions for them.
 */
class ComponentHolder {
public:
	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	GET std::weak_ptr<T> get_component() noexcept {
		for (const std::shared_ptr<Component>& component : components) {
			if (auto ret{std::dynamic_pointer_cast<T>(component)}) {
				return std::weak_ptr<T>{ret};
			}
		}
		return std::weak_ptr<T>{};
	}

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	GET std::set<std::weak_ptr<T>, std::owner_less<std::weak_ptr<T>>> get_components() noexcept {
		std::set<std::weak_ptr<T>, std::owner_less<std::weak_ptr<T>>> ret_comps{};
		for (const std::shared_ptr<Component>& component : components) {
			if (auto ret{std::dynamic_pointer_cast<T>(component)}) {
				ret_comps.insert(ret);
			}
		}
		return ret_comps;
	}

	GET std::set<std::weak_ptr<Component>, std::owner_less<std::weak_ptr<Component>>> get_all_components() noexcept {
		return components | std::views::transform([&](const std::shared_ptr<Component>& sp) {
			return std::weak_ptr<Component>{sp};
		}) | std::ranges::to<std::set<std::weak_ptr<Component>, std::owner_less<std::weak_ptr<Component>>>>();
	}

protected:
	std::unordered_set<std::shared_ptr<Component>> components{}; //! HAS TO BE POINTER SO WE CAN CAST TO SUBCLASSES
};

class GraphicsComponent;

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

	virtual bool operator==(const Component &component) const noexcept;
	virtual void operator=(const Component &component) noexcept;

	void base_render_editor_gui_section() override;

protected:
	Transform transform;
	Component* parent{nullptr};

	void remove_this_from_parents_children() {
		if (parent == nullptr) return;
		if (auto it{std::ranges::find_if(parent->components, [&](const std::shared_ptr<Component>& c) {
			return c.get() == this;
		})}; it != components.end()) {
			parent->components.erase(it);
		}
	}
};
