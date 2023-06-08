#pragma once

#include "Component.h"

class SpotlightComponent : public Component {
public:
	SpotlightComponent();

	virtual bool operator==(const SpotlightComponent &component) const noexcept;
	virtual void operator=(const SpotlightComponent &component) noexcept;

	struct SLData {
		FVector3 direction;
		FVector4 diffuse;
		FVector4 specular;
	} data;

	static const Type component_type = Type::SpotlightComponent;
};

