#include "Engine.h"
#include "Objects/MyCamera.h"
#include "Shapes/Cube.h"

//int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
int main() {
	if (FAILED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED))) {
		OutputDebugStringA("Failed to initialize COM");
		return 1;
	}

	Engine engine;

	engine.world.active_scene->add_character(std::make_shared<MyCamera>());

	std::shared_ptr<Object> obj3 = std::make_shared<Object>();
	engine.world.active_scene->add_object(obj3);
	obj3->add_component(std::make_shared<StaticMeshComponent>(Mesh("statue.obj")));
	obj3->get_component<StaticMeshComponent>()->set_size(FVector3(0.3f, 0.3f, 0.3f));
	obj3->translate(FVector3(50.0f, 0.0f, 50.0f));

	std::shared_ptr<Object> model = std::make_shared<Object>();
	engine.world.active_scene->add_object(model);
	model->add_component(std::make_shared<StaticMeshComponent>(Mesh("statue.obj")));
	model->get_component<StaticMeshComponent>()->set_size(FVector3(0.3f, 0.3f, 0.3f));
	//model->get_component<StaticMeshComponent>()->get_material().albedo_texture = Texture("texture.png");

	/*std::shared_ptr<Object> obj2 = std::make_shared<Object>();
	engine.world.active_scene->add_object(obj2);
	obj2->add_component(std::make_shared<StaticMeshComponent>(Mesh("Shapes/cube.obj")));
	obj2->add_component(std::make_shared<BoxComponent>());
	obj2->translate(FVector3(3.0f, 0.0f, 5.0f));*/

	//engine.renderer.scene.sky.sky.albedo_texture = Texture("Skybox.png");

	engine.compile();
	engine.loop();
}
