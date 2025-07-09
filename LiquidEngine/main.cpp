#include "Engine.h"
#include "TestObjects/MyCamera.h"
#include "Shapes/Cube.h"
#include "TestObjects/MyStatue.h"
#include "TestObjects/MyShaderCube.h"

int main() {
	try {
		if (FAILED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED))) {
			std::cerr << "Failed to initialize COM";
			return 1;
		}

		Engine engine;

		engine.world.active_scene->add_character(std::make_unique<MyCamera>());

		Object* sun = engine.world.active_scene->add_object(std::make_unique<Object>());
		sun->add_component(std::make_unique<DirectionalLightComponent>());
		
		Object* sky = engine.world.active_scene->add_object(std::make_unique<Object>());
		sky->add_component(std::make_unique<SkyComponent>());
		sky->get_component<SkyComponent>()->get_albedo_texture().set_texture("Skybox.png");
		
		Object* obj = engine.world.active_scene->add_object(std::make_unique<MyShaderCube>());

		engine.compile();
		engine.loop();
	} catch (const std::exception& e) {
		std::cerr << e.what();
	}
}
