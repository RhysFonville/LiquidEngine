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

	std::vector<Object> & get_objects() noexcept;
	void add_object(Object object) noexcept;

	void remove_object(int index) noexcept;

	GraphicsScene graphics_scene;
	//PhysicsScene physics_scene;
	//BehaviorManager behavior_manager;

private:
	std::vector<Object> objects;
};

