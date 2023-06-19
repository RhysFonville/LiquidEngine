<h2>Template Behavior Class</h2>

<h3>main.cpp</h3>

```
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

	engine.scene.behavior_manager.object_behaviors.push_back(
		std::make_shared<CameraController>(&engine.scene.get_objects()[0])
	);

	engine.scene.behavior_manager.object_behaviors.push_back(
		std::make_shared<MyBehavior>(&engine.scene.get_objects()[2])
	);

	engine.scene.compile();

	engine.loop();
}

```

<h3>MyBehavior.h</h3>

```
#pragma once

#include "ObjectBehavior.h"

class MyBehavior : public ObjectBehavior {
public:
	MyBehavior(Object* object);

	void tick(float dt) override;
	
```

<h3>MyBehavior.cpp</h3>

```
#include "MyBehavior.h"

MyBehavior::MyBehavior(Object* object) : ObjectBehavior(object) { }

void MyBehavior::tick(float dt) {
	object->rotate(FVector3(1.0f, 1.0f, 1.0f) * dt);
}

```
