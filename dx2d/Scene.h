#pragma once

#include <vector>
#include "GraphicsScene.h"
#include "PhysicsScene.h"
#include "Object.h"
#include "BehaviorManager.h"
#include "Window.h"
#include "Storage.h"

class Scene {
public:
	Scene(Window &window);
	Scene() { };

	void tick();

	void clean_up();

	void compile();

	void read_obj_file(std::string obj_file_path);

	GraphicsScene graphics_scene;
	//PhysicsScene physics_scene;
	std::vector<std::unique_ptr<Object>> objects;
	BehaviorManager behavior_manager;
};

