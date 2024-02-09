#pragma once

#include "LightComponent.h"

/**
* Component for a directional light.
*/
class DirectionalLightComponent : public LightComponent {
public:
	DirectionalLightComponent();

	bool operator==(const DirectionalLightComponent &component) const noexcept;
	void operator=(const DirectionalLightComponent &component) noexcept;

	static const Type component_type = Type::DirectionalLightComponent;

	FVector3 get_direction() const noexcept { return direction; }
	void set_direction(const FVector3 &direction) noexcept { this->direction = direction; changed = true; }

private:
	FVector3 direction = FVector3(0.25f, 0.5f, -1.0f);
};
