#include "Engine.h"

Engine::Engine(HINSTANCE hInstance)
	: scene(Scene()), window(Window(hInstance, std::make_shared<GraphicsScene>(scene.graphics_scene))) {
	window.set_up_window();
	scene = Scene(std::make_shared<Window>(window));
}

void Engine::loop() {
	while (running) {
		window.check_input();
		if (!window.is_running())
			running = false;

		scene.tick();
	}
}
