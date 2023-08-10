#include "Engine.h"
#include "Objects/MyCamera.h"
#include "Objects/Engine/DefaultCube.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	Engine engine(hInstance);

	engine.world.active_scene->add_object(std::make_shared<MyCamera>());
	engine.world.active_scene->add_object(std::make_shared<DefaultCube>());

	engine.world.compile();

	engine.loop();
}
