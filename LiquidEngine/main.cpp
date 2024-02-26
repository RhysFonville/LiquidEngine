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
	sky->get_component<SkyComponent>()->get_albedo_texture().set_texture("Skybox.png");

	//engine.world.active_scene->add_object(std::make_shared<MyStatue>());
	std::shared_ptr<Object> obj = std::make_shared<Object>();
	engine.world.active_scene->add_object(obj);
	obj->add_component(std::make_shared<StaticMeshComponent>(Mesh{"Shapes/cube.obj"}));
	obj->get_component<StaticMeshComponent>()->set_size(FVector3{20.0f, 1.0f, 20.0f});
	obj->get_component<StaticMeshComponent>()->set_position(FVector3{0.0f, -2.0f, 0.0f});

	engine.compile();
	engine.loop();
}
