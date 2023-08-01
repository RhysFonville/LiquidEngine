#pragma once

#include "Component.h"

class PointLightComponent : public Component {
public:
	PointLightComponent();

	bool operator==(const PointLightComponent &component) const noexcept;
	void operator=(const PointLightComponent &light) noexcept;

	struct PLData {
		float range = 100.0f;
		FVector3 attenuation = FVector3(0.2f, 0.2f, 0.2f);
		Color diffuse = Color(100, 100, 100, 100);
		Color specular = Color(0, 0, 0, 0);

		bool null = false;
	} data;

	static const Type component_type = Type::PointLightComponent;
};

