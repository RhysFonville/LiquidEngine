#pragma once

#include "LightComponent.h"
#include "../implot/implot.h"

/**
* Component for a point light.
*/
class PointLightComponent : public LightComponent {
public:
	PointLightComponent();

	bool operator==(const PointLightComponent &component) const noexcept;
	void operator=(const PointLightComponent &light) noexcept;

	GET float get_range() const noexcept { return range; }
	void set_range(float range) noexcept { this->range = range; changed = true; }

	GET FVector3 get_attenuation() const noexcept { return attenuation; }
	void set_attenuation(FVector3 attenuation) noexcept { this->attenuation = attenuation; changed = true; }

	static const Type component_type = Type::PointLightComponent;

private:
	void render_light_component_editor_gui_section() override;

	float range = 100.0f;
	FVector3 attenuation = FVector3(0.1f, 0.1f, 0.0f);
};
