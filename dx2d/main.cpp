#include "Engine.h"
#include "MyBehavior.h"
#include "CameraController.h"

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
	Engine engine(hInstance);

	Object camera("Camera");
	camera.add_component(std::make_shared<CameraComponent>());
	engine.scene.objects.push_back(std::make_shared<Object>(camera));

	engine.scene.graphics_scene->camera = camera.get_component<CameraComponent>();

	Object light("Light");
	light.add_component(std::make_shared<DirectionalLightComponent>());
	engine.scene.objects.push_back(std::make_shared<Object>(light));

	engine.scene.graphics_scene->lights.push_back(light.get_component<DirectionalLightComponent>());

	HPE(engine.scene.read_obj_file("bunny.obj"));

	engine.scene.objects[2]->set_position(FVector3(0.0f, 0.0f, 0.0f));
	engine.scene.objects[2]->set_rotation(FVector3(0.0f, 1.0f, 0.0f));

	engine.scene.objects[2]->add_component(std::make_shared<AppearanceComponent>(
		engine.scene.objects[2]->get_component<MeshComponent>()
	));

	/*(*engine.scene.objects)[1]->set_size(FVector3(15.0f, 15.0f, 15.0f));
	(*engine.scene.objects)[1]->mechanics.forces.push_back(Mechanics::Force(FVector3(0.00001f, 0.0f, 0.0f)));
	HPE(engine.scene.read_obj_file("Shapes/cube.obj"));
	(*engine.scene.objects)[2]->set_size(FVector3(15.0f, 15.0f, 15.0f));

	HPE(engine.scene.read_obj_file("bunny.obj"));
	engine.scene.read_obj_file("Shapes/cube.obj");*/

	engine.scene.behavior_manager.behaviors.push_back(
		std::unique_ptr<ObjectBehavior>(
			new CameraController(engine.scene.objects, engine.scene.objects[0])
		)
	);

	engine.scene.behavior_manager.behaviors.push_back(
	std::unique_ptr<ObjectBehavior>(
			new MyBehavior(engine.scene.objects, engine.scene.objects[2])
		)
	);

	engine.scene.compile();

	engine.loop();
}
