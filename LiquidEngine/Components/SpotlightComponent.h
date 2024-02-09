#pragma once

#include "LightComponent.h"

/**
 * Component for a spotlight.
 */
class SpotlightComponent : public LightComponent {
public:
	SpotlightComponent();

	bool operator==(const SpotlightComponent &component) const noexcept;
	void operator=(const SpotlightComponent &component) noexcept;

	static const Type component_type = Type::SpotlightComponent;

	FVector3 get_direction() const noexcept { return direction; }
	void set_direction(const FVector3 &direction) noexcept { this->direction = direction; changed = true; }

private:
	FVector3 direction = FVector3(0.25f, 0.5f, -1.0f);
};

