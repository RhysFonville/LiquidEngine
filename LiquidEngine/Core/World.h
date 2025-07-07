#pragma once

#include "Scene.h"

/**
 * The entire world. Stores scenes.
 * \see Scene
 */
class World {
public:
	World() { }
	World(const std::vector<Scene> &scenes);
	
	void compile();
	void clean_up();
	void tick(float dt);

	std::vector<Scene>::iterator active_scene;
	std::vector<Scene> scenes;
};

