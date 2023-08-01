#pragma once

#include "Component.h"

class DirectionalLightComponent : public Component {
public:
	DirectionalLightComponent();

	bool operator==(const DirectionalLightComponent &component) const noexcept;
	void operator=(const DirectionalLightComponent &component) noexcept;

	struct DLData {
		FVector3 direction = FVector3(0.25f, 0.5f, -1.0f);
		Color diffuse = Color(100, 100, 100, 100);
		Color specular = Color(0, 0, 0, 255);

		bool null = false;
	} data;

	static const Type component_type = Type::DirectionalLightComponent;
};
