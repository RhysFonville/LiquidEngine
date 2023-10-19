#pragma once

#include <vector>
#include "Graphics/GraphicsScene.h"
#include "PhysicsScene.h"
#include "Object.h"
#include "Window.h"
#include "Character.h"
#include "Input/InputListener.h"

class Scene {
public:
	Scene() { }
	Scene(GraphicsScene* graphics_scene);

	void tick(float dt);

	void clean_up();

	void compile();

	GET std::vector<std::shared_ptr<Object>> & get_objects() noexcept;
	void add_object(const std::shared_ptr<Object> &object) noexcept;

	void add_character(const std::shared_ptr<Character> &character) noexcept;

	void remove_object(int index) noexcept;

	//PhysicsScene physics_scene;
	GraphicsScene* graphics_scene = nullptr;

	Input::InputListener input_listener;
private:
	std::vector<std::shared_ptr<Object>> objects = { };
};

