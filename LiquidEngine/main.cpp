#include "Engine.h"
#include "Objects/MyCamera.h"
#include "Shapes/Cube.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	HRESULT hr = CoInitializeEx(nullptr, COINITBASE_MULTITHREADED);
	if (FAILED(hr))
		throw std::exception("Failed to initialize COM");
	
	Engine engine(hInstance);

	engine.world.active_scene->add_object(std::make_shared<MyCamera>());
	engine.world.active_scene->add_object(std::make_shared<DefaultShapes::Cube>());

	engine.compile();
	engine.loop();
}
