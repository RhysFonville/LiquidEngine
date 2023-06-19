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

	GET std::vector<Object> & get_objects() noexcept;
	void add_object(Object object) noexcept;

	void remove_object(int index) noexcept;

	GET float get_delta_time() const noexcept;

	GraphicsScene graphics_scene;
	//PhysicsScene physics_scene;
	BehaviorManager behavior_manager;

private:
	std::vector<Object> objects;

	struct DeltaTime {
		float dt = 0.0f;
		std::chrono::system_clock::time_point tp1 = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point tp2 = tp1;
	} dt;
};

