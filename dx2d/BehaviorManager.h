#pragma once

#include <memory>
#include "ObjectBehavior.h"

class BehaviorManager {
public:
	BehaviorManager();

	void tick(float dt);

	std::vector<std::shared_ptr<ObjectBehavior>> object_behaviors; // For casting
};

