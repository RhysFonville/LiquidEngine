#pragma once

#include "GraphicsComponent.h"

/**
* Main class for the light components.
*/
class LightComponent : public GraphicsComponent {
public:
	LightComponent(Type type);

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	static bool is_light_component(const T &component) {
		return ((component.get_type() == Component::Type::DirectionalLightComponent) ||
			(component.get_type() == Component::Type::PointLightComponent) ||
			(component.get_type() == Component::Type::SpotlightComponent));
	}

	GET Color get_albedo() const noexcept { return albedo; }
	void set_albedo(const Color &albedo) { this->albedo = albedo; changed = true; }

	GET Color get_specular() const noexcept { return specular; }
	void set_specular(const Color &specular) { this->specular = specular; changed = true; }

	GET bool is_null() const noexcept { return null; }
	void is_null(bool null) noexcept { this->null = null; changed = true; }

protected:
	void render_editor_gui_section() override;
	virtual void render_light_component_editor_gui_section() { }

	Color albedo = Color(255, 255, 255, 255);
	Color specular = Color(50, 50, 50, 255);
	bool null = false;
};
