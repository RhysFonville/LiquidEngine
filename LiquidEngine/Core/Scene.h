#pragma once

#include "../Window.h"
#include "../Input/InputListener.h"
#include "Object.h"
#include "Character.h"

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

	GET std::set<Object*> get_objects() const noexcept;

	Object* add_object(std::unique_ptr<Object>&& object);
	Character* add_character(std::unique_ptr<Character>&& character);

	void render_editor_gui_section();

	//PhysicsScene physics_scene{};
	GraphicsScene* graphics_scene{nullptr};

	Input::InputListener input_listener;

private:
	friend class Object;
	std::set<std::unique_ptr<Object>> objects{};
};
