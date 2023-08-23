#pragma once

#include "LightComponent.h"

class SpotlightComponent : public LightComponent {
public:
	SpotlightComponent();

	virtual bool operator==(const SpotlightComponent &component) const noexcept;
	virtual void operator=(const SpotlightComponent &component) noexcept;

	FVector3 direction = FVector3(0.0f, 0.0f, 0.0f);

	bool null = false;

	static const Type component_type = Type::SpotlightComponent;
};

