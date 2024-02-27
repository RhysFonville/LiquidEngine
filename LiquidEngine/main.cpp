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

	std::shared_ptr<Object> light1 = std::make_shared<Object>();
	engine.world.active_scene->add_object(light1);
	light1->add_component(std::make_shared<PointLightComponent>());
	light1->set_position(FVector3{-20.0f, 1.0f, 0.0f});

	std::shared_ptr<Object> light2 = std::make_shared<Object>();
	engine.world.active_scene->add_object(light2);
	light2->add_component(std::make_shared<PointLightComponent>());
	light2->set_position(FVector3{0.0f, 1.0f, 0.0f});

	std::shared_ptr<Object> light3 = std::make_shared<Object>();
	engine.world.active_scene->add_object(light3);
	light3->add_component(std::make_shared<PointLightComponent>());
	light3->set_position(FVector3{20.0f, 1.0f, 0.0f});

	std::shared_ptr<Object> sky = std::make_shared<Object>();
	engine.world.active_scene->add_object(sky);
	sky->add_component(std::make_shared<SkyComponent>());
	sky->get_component<SkyComponent>()->set_albedo(Color{0, 0, 0, 255});

	std::shared_ptr<Object> obj = std::make_shared<Object>();
	engine.world.active_scene->add_object(obj);
	obj->add_component(std::make_shared<StaticMeshComponent>(Mesh{"floor.obj"}));
	obj->set_size(FVector3{2.0f, 1.0f, 2.0f});
	obj->get_component<StaticMeshComponent>()->get_material().get_albedo_texture().set_texture("wood.png");
	obj->get_component<StaticMeshComponent>()->get_material().set_shininess(2.0f);
	obj->get_component<StaticMeshComponent>()->get_material().set_specular(Color{100, 100, 100, 255});

	engine.compile();
	engine.loop();
}
