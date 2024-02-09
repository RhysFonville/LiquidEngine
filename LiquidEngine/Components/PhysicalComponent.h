#pragma once

#include "Component.h"
#include "../PhysicsBody.h"

/**
* Base component for any component that has physics capabilities.
*/
class PhysicalComponent : public Component {
public:
	PhysicalComponent(Type type) : Component(type) { }

	PhysicsBody physics_body;
};
