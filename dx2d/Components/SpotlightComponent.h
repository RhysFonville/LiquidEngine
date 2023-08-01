#pragma once

#include "Component.h"

class SpotlightComponent : public Component {
public:
	SpotlightComponent();

	virtual bool operator==(const SpotlightComponent &component) const noexcept;
	virtual void operator=(const SpotlightComponent &component) noexcept;

	struct SLData {
		FVector3 direction = FVector3(0.0f, 0.0f, 0.0f);
		Color diffuse = Color(100, 100, 100, 100);
		Color specular = Color(0, 0, 0, 0);

		bool null = false;
	} data;

	static const Type component_type = Type::SpotlightComponent;
};

