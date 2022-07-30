#pragma once

#include "Scene.h"
#include "Window.h"

class Engine {
public:
	Engine(HINSTANCE hInstance);

	void loop();
	
	void clean_up();

	Scene scene;
	Window window;

	bool running = true;
};

