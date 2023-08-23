#pragma once

#include "LightComponent.h"

class DirectionalLightComponent : public LightComponent {
public:
	DirectionalLightComponent();

	bool operator==(const DirectionalLightComponent &component) const noexcept;
	void operator=(const DirectionalLightComponent &component) noexcept;

	FVector3 direction = FVector3(0.25f, 0.5f, -1.0f);
	bool null = false;

	static const Type component_type = Type::DirectionalLightComponent;
};
