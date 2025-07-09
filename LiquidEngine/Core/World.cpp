#include "World.h"

void World::clean_up() {
	if (active_scene != scenes.end()) {
		active_scene->clean_up();
	}
}

void World::compile() {
	if (active_scene != scenes.end()) {
		active_scene->compile();
	}
}

void World::tick(float dt) {
	if (active_scene != scenes.end()) {
		active_scene->tick(dt);
	}
}
