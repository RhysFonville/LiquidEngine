#pragma once

#include "Component.h"
#include "../Physics/PhysicsBody.h"

/**
* Base component for any component that has physics capabilities.
*/
class PhysicalComponent : public Component {
public:
	PhysicalComponent(Type type) : Component(type) { }
	
	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	static bool is_physical_component(const T &component) {
		return ((component.get_type() == Component::Type::CameraComponent) ||
			(component.get_type() == Component::Type::DirectionalLightComponent) ||
			(component.get_type() == Component::Type::PointLightComponent) ||
			(component.get_type() == Component::Type::SpotlightComponent) ||
			(component.get_type() == Component::Type::StaticMeshComponent) ||
			(component.get_type() == Component::Type::SkyComponent));
	}

	void tick(float dt) override;

	PhysicsBody physics_body;

protected:
	void render_editor_gui_section() override;
};
