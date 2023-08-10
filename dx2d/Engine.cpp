#include "Engine.h"

Engine::Engine(HINSTANCE hInstance)
	: world(World()), window(Window(hInstance)), renderer(Renderer(window.get_window())) {
	world.scenes.push_back(Scene(&renderer.scene));
	window.set_up_window();
}

void Engine::compile() {
	world.compile();
	renderer.compile();
}

void Engine::loop() {
	while (running) {
		dt.tp2 = std::chrono::system_clock::now();
		std::chrono::duration<float> elapsed_time = dt.tp2 - dt.tp1;
		dt.tp1 = dt.tp2;
		dt.dt = elapsed_time.count();

		window.check_input();
		if (!window.is_running()) {
			clean_up();
			running = false;
			return;
		}

		world.tick(dt.dt);
		renderer.tick();
	}
}

void Engine::clean_up() {
	world.clean_up();
	window.clean_up();
	renderer.clean_up();
}

float Engine::get_delta_time() const noexcept {
	return dt.dt;
}
