#include "BehaviorManager.h"

BehaviorManager::BehaviorManager() : behaviors(std::vector<std::unique_ptr<ObjectBehavior>>()) {}

void BehaviorManager::tick() {
	for (std::unique_ptr<ObjectBehavior> &behavior : behaviors) {
		behavior->base_tick();
	}
}

void BehaviorManager::clean_up() noexcept {
	for (std::unique_ptr<ObjectBehavior> &behavior : behaviors) {
		behavior.release();
	}
}
