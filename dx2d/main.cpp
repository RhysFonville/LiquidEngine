#include "Engine.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	Engine engine(hInstance);

	/*Object camera("Camera");
	camera.(CameraComponent());
	camera.add_component(PointLightComponent());
	engine.scene.add_object(camera);

	engine.scene.graphics_scene.camera = camera.get_component<CameraComponent>();
	engine.scene.graphics_scene.lights.push_back((Component*)camera.get_component<PointLightComponent>());

	Object light("Light");
	light.add_component(DirectionalLightComponent());
	engine.scene.add_object(light);

	engine.scene.graphics_scene.lights.push_back((Component*)light.get_component<DirectionalLightComponent>());

	HPE(engine.scene.read_obj_file("bunny.obj"));

	engine.scene.get_objects()[2].add_component(AppearanceComponent(
		engine.scene.get_objects()[2].get_component<MeshComponent>()
	));

	engine.scene.behavior_manager.object_behaviors.push_back(
		std::make_shared<CameraController>(&engine.scene.get_objects()[0])
	);

	engine.scene.behavior_manager.object_behaviors.push_back(
		std::make_shared<MyBehavior>(&engine.scene.get_objects()[2])
	);*/

	engine.scene.compile();

	engine.loop();
}
