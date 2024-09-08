#pragma once

#include "OverlapHandler.h"
#include "../Components/PhysicalComponent.h"

class InteractionHandler {
public:
	InteractionHandler() { }

	void handle_interaction(PhysicalComponent* obj1, PhysicalComponent* obj2, float dt) noexcept;

private:
	OverlapHandler overlap_handler{};
};
