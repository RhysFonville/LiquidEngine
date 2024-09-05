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

	std::shared_ptr<Object> sky = std::make_shared<Object>();
	engine.world.active_scene->add_object(sky);
	sky->add_component(std::make_shared<SkyComponent>());
	sky->get_component<SkyComponent>()->get_albedo_texture().set_texture("Skybox.png");

	std::shared_ptr<Object> obj = std::make_shared<Object>();
	engine.world.active_scene->add_object(obj);
	obj->add_component(std::make_shared<StaticMeshComponent>(Mesh{"Shapes/cube.obj"}));
	obj->add_component(std::make_shared<BoundingBoxComponent>());
	obj->set_position(FVector3{0.0f, 0.0f, 3.0f});
	obj->mimic_position_component = obj->get_component<BoundingBoxComponent>();
	obj->mimic_rotation_component = obj->get_component<BoundingBoxComponent>();

	std::shared_ptr<Object> obj1 = std::make_shared<Object>();
	engine.world.active_scene->add_object(obj1);
	obj1->add_component(std::make_shared<StaticMeshComponent>(Mesh{"Shapes/cube.obj"}));
	obj1->add_component(std::make_shared<BoundingBoxComponent>());
	obj1->set_position(FVector3{0.0f, 0.0f, -3.0f});
	obj1->get_component<BoundingBoxComponent>()->physics_body.set_mass(1.0f);
	obj1->mimic_position_component = obj1->get_component<BoundingBoxComponent>();
	obj1->mimic_rotation_component = obj1->get_component<BoundingBoxComponent>();

	/*std::shared_ptr<Object> wall1 = std::make_shared<Object>();
	engine.world.active_scene->add_object(wall1);
	wall1->add_component(std::make_shared<StaticMeshComponent>(Mesh{"Shapes/cube.obj"}));
	wall1->add_component(std::make_shared<BoundingBoxComponent>());
	wall1->set_position(FVector3{0.0f, 0.0f, 5.0f});
	wall1->get_component<BoundingBoxComponent>()->physics_body.set_mass(2.0f);
	wall1->mimic_position_component = wall1->get_component<BoundingBoxComponent>();
	wall1->mimic_rotation_component = wall1->get_component<BoundingBoxComponent>();

	std::shared_ptr<Object> wall2 = std::make_shared<Object>();
	engine.world.active_scene->add_object(wall2);
	wall2->add_component(std::make_shared<StaticMeshComponent>(Mesh{"Shapes/cube.obj"}));
	wall2->add_component(std::make_shared<BoundingBoxComponent>());
	wall2->set_position(FVector3{0.0f, 0.0f, -5.0f});
	wall2->get_component<BoundingBoxComponent>()->physics_body.set_mass(2.0f);
	wall2->mimic_position_component = wall2->get_component<BoundingBoxComponent>();
	wall2->mimic_rotation_component = wall2->get_component<BoundingBoxComponent>();*/

	engine.compile();
	engine.loop();
}
