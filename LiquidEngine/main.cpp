#include "Engine.h"
#include "Objects/MyCamera.h"
#include "Shapes/Cube.h"
#include "Objects/MyStatue.h"

int main() {
	if (FAILED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED))) {
		OutputDebugStringA("Failed to initialize COM");
		return 1;
	}

	Engine engine;

	engine.world.active_scene->add_character(std::make_shared<MyCamera>());

	//std::shared_ptr<Object> light = std::make_shared<Object>();
	//engine.world.active_scene->add_object(light);
	//light->add_component(std::make_shared<DirectionalLightComponent>());

	std::shared_ptr<Object> sky = std::make_shared<Object>();
	engine.world.active_scene->add_object(sky);
	sky->add_component(std::make_shared<SkyComponent>());
	sky->get_component<SkyComponent>()->set_albedo(Color{0, 0, 0, 255});

	//engine.world.active_scene->add_object(std::make_shared<MyStatue>());
	std::shared_ptr<Object> obj = std::make_shared<Object>();
	engine.world.active_scene->add_object(obj);
	obj->add_component(std::make_shared<StaticMeshComponent>(Mesh{"floor.obj"}));
	obj->get_component<StaticMeshComponent>()->get_material().get_albedo_texture().set_texture("wood.png");

	engine.compile();
	engine.loop();
}
