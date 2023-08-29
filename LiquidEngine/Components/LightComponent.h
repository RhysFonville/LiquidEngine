#pragma once

#include "GraphicsComponent.h"

class LightComponent : public GraphicsComponent {
public:
	LightComponent(Type type);

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	static bool is_light_component(const T &component) {
		return ((component.get_type() == Component::Type::DirectionalLightComponent) ||
			(component.get_type() == Component::Type::PointLightComponent) ||
			(component.get_type() == Component::Type::SpotlightComponent));
	}

	Color albedo = Color(255, 255, 255, 255);
	Color specular = Color(0, 0, 0, 255);
};
