#pragma once

#include "ObjectBehavior.h"

class BehaviorManager {
public:
	BehaviorManager();

	void tick();

	void clean_up() noexcept;

	std::vector<std::unique_ptr<ObjectBehavior>> behaviors;
};

