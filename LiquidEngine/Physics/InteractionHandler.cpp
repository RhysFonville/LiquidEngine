#include "InteractionHandler.h"

void InteractionHandler::handle_interaction(PhysicalComponent* obj1, PhysicalComponent* obj2) noexcept {
	overlap_handler.handle_overlap(obj1, obj2);
}
