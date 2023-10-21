#pragma once

#include "GraphicsComponent.h"
#include "../PhysicsBody.h"

class PhysicalComponent : public Component {
public:
	PhysicalComponent(Type type) : Component(type) { }

	PhysicsBody physics_body;
};
