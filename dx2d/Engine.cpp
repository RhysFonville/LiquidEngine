#include "Engine.h"

Engine::Engine(HINSTANCE hInstance)
	: scene(Scene()), window(Window(hInstance)) {
	window.set_up_window();
	scene = Scene(std::make_shared<Window>(window));
	window.graphics_scene = std::make_shared<GraphicsScene>(scene.graphics_scene);
}

void Engine::loop() {
	while (running) {
		window.check_input();
		if (!window.is_running()) {
			running = false;
			return;
		}

		scene.tick();
	}
}
