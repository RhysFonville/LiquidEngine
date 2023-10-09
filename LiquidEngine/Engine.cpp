#include "Engine.h"

Engine::Engine(HINSTANCE hInstance)
	: world(World()), window(Window(hInstance)) {
	// Init COM
//#if (_WIN32_WINNT >= 0x0A00 /*_WIN32_WINNT_WIN10*/)
//	Microsoft::WRL::Wrappers::RoInitializeWrapper initialize(RO_INIT_MULTITHREADED);
//	if (FAILED(initialize))
//		throw std::exception("Failed to initialize COM");
//#else
	//HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	//if (FAILED(hr))
		//throw std::exception("Failed to initialize COM");
//#endif

	window.set_up_window();
	renderer = Renderer(window.get_window());

	world.scenes.push_back(Scene(&renderer.scene));
	world.active_scene = world.scenes.begin();
}

void Engine::compile() {
	world.compile();
	renderer.compile();
}

void Engine::loop() {
	try {
		while (running) {
			dt.tp2 = std::chrono::system_clock::now();
			std::chrono::duration<float> elapsed_time = dt.tp2 - dt.tp1;
			dt.tp1 = dt.tp2;
			dt.dt = elapsed_time.count();

			world.tick(dt.dt);
			renderer.tick();

			if (EngineToggles::terminate) {
				EngineToggles::terminate = false;
				throw;
			}

			window.check_input();
			if (!window.is_running()) {
				running = false;
				clean_up();
				return;
			}
		}
	} catch (const std::exception &e) {
		if (!std::string(e.what()).empty()) OutputDebugStringA(e.what());
		running = false;
		SendMessageA(window.get_window(), WM_QUIT, 0, 0);
		clean_up();
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
