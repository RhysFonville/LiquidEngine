#pragma once

#include "PhysicalComponent.h"

/**
* Component for a box used for physics.
*/
class BoxComponent : public PhysicalComponent {
public:
	BoxComponent() : PhysicalComponent{} { }

	SimpleBox box{};
};
