#pragma once

#include "Component.h"

class DirectionalLightComponent : public Component {
public:
	DirectionalLightComponent();

	bool operator==(const DirectionalLightComponent &component) const noexcept;
	void operator=(const DirectionalLightComponent &component) noexcept;

	struct DLData {
		FVector3 direction = FVector3(0.25f, 0.5f, -1.0f);
		FVector4 diffuse = FVector4(255.0f, 255.0f, 255.0f, 255.0f);
		FVector4 specular = FVector4(0.5f, 0.5f, 0.5f, 1.0f);
	} data;

	static const Type component_type = Type::DirectionalLightComponent;
};
