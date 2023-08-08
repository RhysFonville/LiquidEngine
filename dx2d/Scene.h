#pragma once

#include <vector>
#include "Graphics/Renderer.h"
#include "PhysicsScene.h"
#include "Object.h"
#include "Window.h"

class Scene {
public:
	Scene(Window &window);
	Scene() { };

	void tick();

	void clean_up();

	void compile();

	//void read_obj_file(std::string obj_file_path);

	GET std::vector<std::shared_ptr<Object>> & get_objects() noexcept;
	void add_object(const std::shared_ptr<Object> &object) noexcept;

	void remove_object(int index) noexcept;

	GET float get_delta_time() const noexcept;

	Renderer graphics_scene;
	//PhysicsScene physics_scene;

private:
	std::vector<std::shared_ptr<Object>> objects;

	struct DeltaTime {
		float dt = 0.0f;
		std::chrono::system_clock::time_point tp1 = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point tp2 = tp1;
	} dt;
};

