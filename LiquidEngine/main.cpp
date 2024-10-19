#include "Engine.h"
#include "Objects/MyCamera.h"
#include "Shapes/Cube.h"
#include "Objects/MyStatue.h"
#include "Objects/MyShaderCube.h"

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

	std::shared_ptr<Object> obj = std::make_shared<MyShaderCube>();
	engine.world.active_scene->add_object(obj);

	/*std::shared_ptr<Object> obj1 = std::make_shared<Object>();
	engine.world.active_scene->add_object(obj1);
	obj1->add_component(std::make_shared<StaticMeshComponent>(Mesh{"crate.obj"}, Material{"Graphics/DefaultVertex.hlsl", "Graphics/UnlitPixel.hlsl"}));
	obj1->add_component(std::make_shared<BoundingBoxComponent>());
	obj1->set_position(FVector3{0.0f, 0.0f, -3.0f});
	obj1->get_component<BoundingBoxComponent>()->physics_body.set_mass(1.0f);
	obj1->mimic_position_component = obj1->get_component<BoundingBoxComponent>();
	obj1->mimic_rotation_component = obj1->get_component<BoundingBoxComponent>();
	obj1->get_component<StaticMeshComponent>()->get_material().get_albedo_texture().set_texture("transparent.png");*/

	engine.compile();
	engine.loop();
}
