#pragma once

#include <list>
#include "Scene.h"

/**
 * The entire world. Stores scenes.
 * \see Scene
 */
class World {
public:
	World() { }
	
	void compile();
	void clean_up();
	void tick(float dt);

	std::list<Scene>::iterator active_scene;
	std::list<Scene> scenes;
};
