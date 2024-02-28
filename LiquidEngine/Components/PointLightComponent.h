#pragma once

#include "LightComponent.h"

/**
* Component for a point light.
*/
class PointLightComponent : public LightComponent {
public:
	PointLightComponent();

	bool operator==(const PointLightComponent &component) const noexcept;
	void operator=(const PointLightComponent &light) noexcept;

	static const Type component_type = Type::PointLightComponent;

	float get_range() const noexcept { return range; }
	void set_range(float range) noexcept { this->range = range; changed = true; }

	FVector3 get_attenuation() const noexcept { return attenuation; }
	void set_attenuation(FVector3 attenuation) noexcept { this->attenuation = attenuation; changed = true; }

private:
	float range = 100.0f;
	FVector3 attenuation = FVector3(0.0f, 0.2f, 0.5f);
};
