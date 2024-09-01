#include "PhysicsScene.h"

void PhysicsScene::tick() {
	for (int i = 0; i < objects.size(); i++) {
		if (!objects[i]->physics_body.frozen) {
			for (int j = i+1; j < objects.size(); j++) {
				overlap_handler.handle_overlap(objects[i], objects[j]);
			}
		}
	}
}

void PhysicsScene::clean_up() {
	objects.clear();
}
