#pragma once

#include "Component.h"
#include "PhysicsBody.h"

class PhysicalComponent : public Component {
public:
	PhysicalComponent() { }
	PhysicalComponent(Type type) : Component(type) { }

	PhysicsBody physics_body;
};

