#pragma once

#include "Component.h"

class GraphicsTracker {
public:
	bool has_changed() const noexcept { return changed; }
	void has_changed(bool changed) noexcept { this->changed = changed; }

	bool needs_compile() const noexcept { return compile; }
	void needs_compile(bool compile) noexcept { this->compile = compile; }

protected:
	bool changed{false};
	bool compile{false};
};

/**
* Main component for any component that is used for rendering and graphics.
*/
class GraphicsComponent : public Component, public GraphicsTracker {
public:
	GraphicsComponent(Type type);

	//template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	//static bool is_graphics_component(const T &component) {
	//	return ((component.get_type() == Component::Type::CameraComponent) ||
	//		(component.get_type() == Component::Type::DirectionalLightComponent) ||
	//		(component.get_type() == Component::Type::PointLightComponent) ||
	//		(component.get_type() == Component::Type::SpotlightComponent) ||
	//		(component.get_type() == Component::Type::StaticMeshComponent) ||
	//		(component.get_type() == Component::Type::SkyComponent));
	//}

	virtual void set_position(FVector3 position) noexcept override {
		if (position == transform.position) return;
		Component::set_position(position);
		changed = true;
	}
	virtual void set_rotation(FVector3 rotation) noexcept {
		if (rotation == transform.rotation) return;
		Component::set_rotation(rotation);
		changed = true;
	}
	virtual void set_size(FVector3 size) noexcept {
		if (size == transform.size) return;
		Component::set_size(size);
		changed = true;
	}
};
