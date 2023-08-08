#include "Engine.h"
#include "Objects/MyCamera.h"
#include "Objects/Engine/DefaultCube.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	Engine engine(hInstance);

	engine.scene.add_object(std::shared_ptr<MyCamera>());
	engine.scene.add_object(std::shared_ptr<DefaultCube>());

	engine.scene.compile();

	engine.loop();
}
