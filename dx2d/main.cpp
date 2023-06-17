#include "Engine.h"
#include "MyBehavior.h"
#include "CameraController.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	Engine engine(hInstance);

	Object camera("Camera");
	camera.add_component(CameraComponent());
	engine.scene.add_object(camera);

	engine.scene.graphics_scene.camera = camera.get_component<CameraComponent>();

	Object light("Light");
	light.add_component(DirectionalLightComponent());
	engine.scene.add_object(light);

	engine.scene.graphics_scene.lights.push_back((Component*)light.get_component<DirectionalLightComponent>());

	HPE(engine.scene.read_obj_file("bunny.obj"));

	engine.scene.get_objects()[2].add_component(AppearanceComponent(
		engine.scene.get_objects()[2].get_component<MeshComponent>()
	));

	/*engine.scene.behavior_manager.behaviors.push_back(
		std::unique_ptr<ObjectBehavior>(
			new CameraController(engine.scene.objects, engine.scene.objects[0].get())
		)
	);

	engine.scene.behavior_manager.behaviors.push_back(
		std::unique_ptr<ObjectBehavior>(
			new MyBehavior(engine.scene.objects, engine.scene.objects[2].get())
		)
	);*/

	engine.scene.compile();

	engine.loop();
}
