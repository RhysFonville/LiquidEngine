#pragma once

#include "SceneComponents/World.h"
#include "Window.h"
#include "StaticEngineProcessToggles.h"

/**
 * The main LiquidEngine class.
 */
class Engine {
public:
	Engine();

	void compile();
	void loop();
	void clean_up();

	GET float get_delta_time() const noexcept;

	World world{};
	Window window{};
	Renderer renderer{};

	bool running = true;

private:
	struct DeltaTime {
		float dt = 0.0f;
		std::chrono::system_clock::time_point tp1 = std::chrono::system_clock::now();
		std::chrono::system_clock::time_point tp2 = tp1;
	} dt;
};

