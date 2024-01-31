#pragma once

#include "LightComponent.h"

class PointLightComponent : public LightComponent {
public:
	PointLightComponent();

	bool operator==(const PointLightComponent &component) const noexcept;
	void operator=(const PointLightComponent &light) noexcept;

	float range = 100.0f;
	FVector3 attenuation = FVector3(0.0f, 0.2f, 0.0f);
	bool null = false;

	static const Type component_type = Type::PointLightComponent;
};
