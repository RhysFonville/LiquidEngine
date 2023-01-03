#include "Engine.h"
#include "MyBehavior.h"
#include "CameraController.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	Engine engine(hInstance);
	try {
		Object camera("Camera");
		camera.components.push_back(std::make_shared<CameraComponent>());
		camera.components.push_back(std::make_shared<PointLightComponent>());
		engine.scene.objects->push_back(std::make_shared<Object>(camera));

		/*Object light("Light");
		light.components.push_back(std::make_shared<PointLightComponent>());
		engine.scene.objects->push_back(std::make_shared<Object>(light));*/

		camera.get_component<PointLightComponent>()->range = 100.0f;
		camera.get_component<PointLightComponent>()->attenuation = FVector3(0.0f, 0.0f, 1.0f);
		camera.get_component<PointLightComponent>()->diffuse = FVector4(255.0f, 255.0f, 255.0f, 255.0f);
		camera.get_component<PointLightComponent>()->specular = FVector4(255.0f, 255.0f, 255.0f, 255.0f);
		
		//camera.get_component<PointLightComponent>()->set_position(FVector3(0.0f, 2.0f, 0.0f));

		/*HANDLE_POSSIBLE_EXCEPTION(engine.scene.read_obj_file("Shapes/cube.obj"));
		(*engine.scene.objects)[1]->set_size(FVector3(15.0f, 15.0f, 15.0f));
		(*engine.scene.objects)[1]->mechanics.forces.push_back(Mechanics::Force(FVector3(0.00001f, 0.0f, 0.0f)));
		HANDLE_POSSIBLE_EXCEPTION(engine.scene.read_obj_file("Shapes/cube.obj"));
		(*engine.scene.objects)[2]->set_size(FVector3(15.0f, 15.0f, 15.0f));*/

		//HANDLE_POSSIBLE_EXCEPTION(engine.scene.read_obj_file("bunny.obj"));
		engine.scene.read_obj_file("Shapes/cube.obj");

		engine.scene.behavior_manager.behaviors.push_back(
			std::unique_ptr<ObjectBehavior>(
				new CameraController(engine.scene.objects, engine.scene.objects->at(0))
			)
		);

		engine.scene.behavior_manager.behaviors.push_back(
		std::unique_ptr<ObjectBehavior>(
				new MyBehavior(engine.scene.objects, engine.scene.objects->at(1))
			)
		);

		engine.scene.compile();

		engine.loop();
	} catch (const std::exception &e) {
		if (YESNO_MESSAGE(L"Do you want to append the error message to the output file?") == true) {
			append_to_file("out.txt", e.what());
		}
	}
}
