#pragma once

#include "LightComponent.h"

/**
* Component for a directional light.
*/
class DirectionalLightComponent : public LightComponent {
public:
	DirectionalLightComponent();

	static const Type component_type = Type::DirectionalLightComponent;
};
