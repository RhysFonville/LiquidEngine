#pragma once

#include "Component.h"

class GraphicsComponent : public Component {
public:
	GraphicsComponent(Type type);

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	static bool is_graphics_component(const T &component) {
		return ((component.get_type() == Component::Type::CameraComponent) ||
			(component.get_type() == Component::Type::DirectionalLightComponent) ||
			(component.get_type() == Component::Type::PointLightComponent) ||
			(component.get_type() == Component::Type::SpotlightComponent) ||
			(component.get_type() == Component::Type::StaticMeshComponent));
	}
};
