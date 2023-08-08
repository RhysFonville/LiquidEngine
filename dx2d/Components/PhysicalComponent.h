#pragma once

#include "GraphicsComponent.h"
#include "../PhysicsBody.h"

class PhysicalComponent : public GraphicsComponent {
public:
	PhysicalComponent(Type type) : GraphicsComponent(type) { }

	PhysicsBody physics_body;
};
