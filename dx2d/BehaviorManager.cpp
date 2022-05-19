#include "BehaviorManager.h"

BehaviorManager::BehaviorManager() { }

void BehaviorManager::tick() {
	for (const std::shared_ptr<ObjectBehavior> &behavior : behaviors) {
		behavior->tick();
	}
}
