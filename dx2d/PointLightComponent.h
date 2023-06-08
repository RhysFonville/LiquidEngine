#pragma once

#include "Component.h"

class PointLightComponent : public Component {
public:
	PointLightComponent();

	bool operator==(const PointLightComponent &component) const noexcept;
	void operator=(const PointLightComponent &light) noexcept;

	struct PLData {
		FVector3 attenuation = FVector3(0.0f, 0.2f, 0.0f);
		float range = 100.0f;
		FVector4 diffuse = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
		FVector4 specular = FVector4(1.0f, 1.0f, 1.0f, 1.0f);
	} data;

	static const Type component_type = Type::PointLightComponent;
};

