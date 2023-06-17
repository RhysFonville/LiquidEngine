#include "Engine.h"

Engine::Engine(HINSTANCE hInstance)
	: scene(Scene()), window(Window(hInstance)) {
	window.set_up_window();
	scene = Scene(window);
	window.graphics_scene = &scene.graphics_scene;
}

void Engine::loop() {
	while (running) {
		window.check_input();
		if (!window.is_running()) {
			clean_up();
			running = false;
			return;
		}

		scene.tick();
	}
}

void Engine::clean_up() {
	scene.clean_up();
	window.clean_up();
}
