#pragma once

#include "ObjectBehavior.h"

class BehaviorManager {
public:
	BehaviorManager();

	void tick();

	std::vector<std::shared_ptr<ObjectBehavior>> behaviors;
};

