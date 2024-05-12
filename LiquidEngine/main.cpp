#include "Engine.h"
#include "Objects/MyCamera.h"
#include "Shapes/Cube.h"
#include "Objects/MyStatue.h"

int main() {
	if (FAILED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED))) {
		*debug_console << "Failed to initialize COM";
		return 1;
	}

	Engine engine;

	engine.world.active_scene->add_character(std::make_shared<MyCamera>());

	std::shared_ptr<Object> sun = std::make_shared<Object>();
	engine.world.active_scene->add_object(sun);
	sun->add_component(std::make_shared<DirectionalLightComponent>());

	/*std::shared_ptr<Object> sky = std::make_shared<Object>();
	engine.world.active_scene->add_object(sky);
	sky->add_component(std::make_shared<SkyComponent>());
	sky->get_component<SkyComponent>()->set_albedo(Color{0, 0, 0, 255});*/

	std::shared_ptr<Object> obj = std::make_shared<Object>();
	engine.world.active_scene->add_object(obj);
	obj->add_component(std::make_shared<StaticMeshComponent>(Mesh{"Shapes/cube.obj"}));
	obj->add_component(std::make_shared<BoxComponent>());
	obj->mimic_position_component = obj->get_component<BoxComponent>();

	std::shared_ptr<Object> obj1 = std::make_shared<Object>();
	engine.world.active_scene->add_object(obj1);
	obj1->add_component(std::make_shared<StaticMeshComponent>(Mesh{"Shapes/cube.obj"}));
	obj1->add_component(std::make_shared<BoxComponent>());
	obj1->set_position(FVector3{0.0f, 0.0f, 3.0f});
	obj1->mimic_position_component = obj1->get_component<BoxComponent>();

	engine.compile();
	engine.loop();
}
