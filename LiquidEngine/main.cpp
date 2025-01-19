#include "Engine.h"
#include "Objects/MyCamera.h"
#include "Shapes/Cube.h"
#include "Objects/MyStatue.h"
#include "Objects/MyShaderCube.h"

int main() {
	try {
		if (FAILED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED))) {
			std::cerr << "Failed to initialize COM";
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
		sky->get_component<SkyComponent>()->get_albedo_texture().set_texture("Skybox.png");*/
		
		std::shared_ptr<Object> obj = std::make_shared<MyShaderCube>();
		engine.world.active_scene->add_object(obj);

		engine.compile();
		engine.loop();
	} catch (const std::exception& e) {
		std::cerr << e.what();
	}
}
