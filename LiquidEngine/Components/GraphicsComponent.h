#pragma once

#include "Component.h"

/**
* Main component for any component that is used for rendering and graphics.
*/
class GraphicsComponent : public Component {
public:
	GraphicsComponent(Type type);

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	static bool is_graphics_component(const T &component) {
		return ((component.get_type() == Component::Type::CameraComponent) ||
			(component.get_type() == Component::Type::DirectionalLightComponent) ||
			(component.get_type() == Component::Type::PointLightComponent) ||
			(component.get_type() == Component::Type::SpotlightComponent) ||
			(component.get_type() == Component::Type::StaticMeshComponent) ||
			(component.get_type() == Component::Type::SkyComponent));
	}

	virtual void set_position(FVector3 position) noexcept override {
		Component::set_position(position);
		changed = true;
	}
	virtual void set_rotation(FVector3 rotation) noexcept {
		Component::set_rotation(rotation);
		changed = true;
	}
	virtual void set_size(FVector3 size) noexcept {
		Component::set_size(size);
		changed = true;
	}

	bool has_changed() const noexcept { return changed; }
	void has_changed(bool changed) noexcept { this->changed = changed; }

protected:
	bool changed{false};
};
