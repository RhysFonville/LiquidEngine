#pragma once

#include <vector>
#include "GraphicsScene.h"
#include "PhysicsScene.h"
#include "Object.h"
#include "BehaviorManager.h"
#include "Window.h"

class Scene {
public:
	Scene(const std::shared_ptr<Window> &window);
	Scene() { };

	void tick();

	GraphicsScene graphics_scene;
	PhysicsScene physics_scene;
	std::shared_ptr<std::vector<Object>> objects;
	BehaviorManager behavior_manager;
};

