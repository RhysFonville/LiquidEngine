#pragma once

#include "Component.h"

class PointLightComponent : public Component {
public:
	PointLightComponent();

	bool operator==(const PointLightComponent &component) const noexcept;
	void operator=(const PointLightComponent &light) noexcept;

	struct PLData {
		float range = 100.0f;
		FVector3 attenuation = FVector3(0.0f, 0.2f, 0.0f);
		Color diffuse = Color(255, 255, 255, 255);
		Color specular = Color(255, 255, 255, 255);

		bool null = false;
	} data;

	static const Type component_type = Type::PointLightComponent;
};

