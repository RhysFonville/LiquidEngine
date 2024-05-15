#pragma once

#include "OverlapHandler.h"

class PhysicsScene {
public:
	PhysicsScene() { }

	void tick();

	void clean_up();

	std::vector<PhysicalComponent*> objects{};

private:
	OverlapHandler overlap_handler{};
};
