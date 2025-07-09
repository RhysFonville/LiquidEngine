#pragma once

#include "GraphicsComponent.h"

/**
* Main class for the light components.
*/
class LightComponent : public GraphicsComponent {
public:
	LightComponent();

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
