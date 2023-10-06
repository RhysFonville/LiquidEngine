#include "Engine.h"
#include "Objects/MyCamera.h"
#include "Shapes/Cube.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	if (FAILED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED))) {
		OutputDebugStringA("Failed to initialize COM");
		return 1;
	}

	Engine engine(hInstance);

	engine.world.active_scene->add_object(std::make_shared<MyCamera>());

	std::shared_ptr<Object> model = std::make_shared<Object>();
	engine.world.active_scene->add_object(model);
	model->add_component(std::make_shared<StaticMeshComponent>(Mesh("statue.obj")));
	model->get_component<StaticMeshComponent>()->set_size(FVector3(0.3f, 0.3f, 0.3f));
	model->get_component<StaticMeshComponent>()->get_material().albedo_texture = Texture("mipmaptest.png");

	engine.renderer.scene.sky.albedo_texture = Texture("Skybox.png");

	engine.compile();
	engine.loop();
}
