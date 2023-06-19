#pragma once

#include "ObjectBehavior.h"

class BehaviorManager {
public:
	BehaviorManager();

	void tick(float dt);

	std::vector<ObjectBehavior*> object_behaviors; // For casting
};

