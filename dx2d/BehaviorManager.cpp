#include "BehaviorManager.h"

BehaviorManager::BehaviorManager() : object_behaviors(std::vector<ObjectBehavior*>()) { }

void BehaviorManager::tick(float dt) {
	for (ObjectBehavior* behavior : object_behaviors) {
		behavior->tick(dt);
	}
}
