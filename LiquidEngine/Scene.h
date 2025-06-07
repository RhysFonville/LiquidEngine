#pragma once

#include <vector>
#include "Object.h"
#include "Window.h"
#include "Character.h"
#include "Input/InputListener.h"

/**
 * Scene to store objects in.
 * \see GraphicsScene
 */
class Scene {
public:
	Scene() { }
	Scene(GraphicsScene* graphics_scene);

	void tick(float dt);

	void clean_up();

	void compile();

	GET std::unordered_set<std::shared_ptr<Object>> & get_objects() noexcept;
	void add_object(const std::shared_ptr<Object> &object) noexcept;

	void add_character(const std::shared_ptr<Character> &character) noexcept;

	void render_editor_gui_section();

	PhysicsScene physics_scene{};
	GraphicsScene* graphics_scene{nullptr};

	Input::InputListener input_listener;

private:
	std::unordered_set<std::shared_ptr<Object>> objects{};
};

