#include "BehaviorManager.h"

BehaviorManager::BehaviorManager() : object_behaviors(std::vector<std::shared_ptr<ObjectBehavior>>()) { }

void BehaviorManager::tick(float dt) {
	for (std::shared_ptr<ObjectBehavior> &behavior : object_behaviors) {
		behavior->tick(dt);
	}
}
