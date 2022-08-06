#include "Engine.h"
#include "MyBehavior.h"
#include "CameraController.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	Engine engine(hInstance);
	try {
		Object camera("Camera");
		camera.components.push_back(std::make_shared<CameraComponent>());
		/*camera.components.push_back(std::make_shared<PointLightComponent>());
		camera.get_component<PointLightComponent>()->range = 100.0f;
		camera.get_component<PointLightComponent>()->attenuation = FVector3(0.1f, 0.05f, 0.0f);
		camera.get_component<PointLightComponent>()->ambient = FVector4(0.0f, 0.0f, 0.0f, 0.0f);
		camera.get_component<PointLightComponent>()->diffuse = FVector4(255.0f, 255.0f, 255.0f, 255.0f);*/
		engine.scene.objects->push_back(std::make_shared<Object>(camera));

		Object sun("Sun");
		sun.components.push_back(std::make_shared<DirectionalLightComponent>());
		engine.scene.objects->push_back(std::make_shared<Object>(sun));

		sun.get_component<DirectionalLightComponent>()->diffuse = FVector4(255, 255, 255, 255);

		engine.scene.read_obj_file("untitled.obj");

		engine.scene.behavior_manager.behaviors.push_back(
			std::unique_ptr<ObjectBehavior>(
				new CameraController(engine.scene.objects, engine.scene.objects->at(0))
			)
		);

		engine.scene.behavior_manager.behaviors.push_back(
		std::unique_ptr<ObjectBehavior>(
				new MyBehavior(engine.scene.objects, engine.scene.objects->at(2))
			)
		);

		engine.scene.graphics_scene->compile();

		engine.loop();
	} catch (const std::exception &e) {
		if (YESNO_MESSAGE(L"Do you want to append this error message to the output file?") == true) {
			engine.clean_up();
			append_to_file("out.txt", e.what());
		}
	}
}
