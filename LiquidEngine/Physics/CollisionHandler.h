#pragma once

#include "../Components/PhysicalComponent.h"

class CollisionHandler {
public:
	CollisionHandler() { }

	void handle_collision(const FVector3 &pos1, PhysicsBody &obj1, const FVector3 &pos2, PhysicsBody &obj2) noexcept;
};
