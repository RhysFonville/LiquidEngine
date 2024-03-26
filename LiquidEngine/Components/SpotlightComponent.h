#pragma once

#include "LightComponent.h"

/**
 * Component for a spotlight.
 */
class SpotlightComponent : public LightComponent {
public:
	SpotlightComponent();

	static const Type component_type = Type::SpotlightComponent;
};

