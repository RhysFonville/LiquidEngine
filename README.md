<h1>Liquid Engine</h1>
<h2>A D3D12-based 3D graphics, physics, and game engine.</h2>

<h3>How Liquid Engine works</h3>

Every object is placed in a scene. Objects have components, which are important attachments you can add to any object to give it functionality.
Components aren't perfectly free and may not fully allow you to get all of the functionality you want out of an object. If this is the case, you can make your own object class! Create a subclass to the Object class and override the needed functions.
Here is an example camera object with input:

MyCamera.h:

```
#pragma once

#include "../Character.h"
#include "../Components/CameraComponent.h"
#include "../Components/PointLightComponent.h"

class MyCamera : public Character {
public:
	MyCamera();

	void on_start() override;
	void tick(float dt) override;

	void on_turn(float dt);

	void on_forward(float dt);
	void on_backward(float dt);
	void on_left(float dt);
	void on_right(float dt);
	void on_up(float dt);
	void on_down(float dt);
	void on_slow(float dt);
	void on_not_slow(float dt);

private:
	POINT current_cursor_pos = { 0, 0 };
	POINT cursor_diff = { 0, 0 };
	POINT previous_cursor_pos = current_cursor_pos;

	float regular_speed = 6.0f;
	float slow_speed = regular_speed / 2.0f;
	float current_speed = regular_speed;

	std::shared_ptr<CameraComponent> camera;
	std::shared_ptr<PointLightComponent> light;
};

```

MyCamera.cpp:

```
#include "MyCamera.h"

MyCamera::MyCamera()
	: camera(std::make_shared<CameraComponent>()),
	light(std::make_shared<PointLightComponent>()) {
	
	keybind_set.categories.push_back({ "Category" });
	keybind_set.categories[0].actions.push_back({ "Move" });

	keybind_set.categories[0].actions[0].binds.push_back(
		{ Input::WindowsKeyCode::W,
		std::function<void(float)>([&](float dt) { on_forward(dt); }),
		Input::CallbackEvent::IsPressed }
	);
	keybind_set.categories[0].actions[0].binds.push_back(
		{ Input::WindowsKeyCode::A,
		std::function<void(float)>([&](float dt) { on_left(dt); }),
		Input::CallbackEvent::IsPressed }
	);
	keybind_set.categories[0].actions[0].binds.push_back(
		{ Input::WindowsKeyCode::S,
		std::function<void(float)>([&](float dt) { on_backward(dt); }),
		Input::CallbackEvent::IsPressed }
	);
	keybind_set.categories[0].actions[0].binds.push_back(
		{ Input::WindowsKeyCode::D,
		std::function<void(float)>([&](float dt) { on_right(dt); }),
		Input::CallbackEvent::IsPressed }
	);
	keybind_set.categories[0].actions[0].binds.push_back(
		{ Input::WindowsKeyCode::Q,
		std::function<void(float)>([&](float dt) { on_down(dt); }),
		Input::CallbackEvent::IsPressed }
	);
	keybind_set.categories[0].actions[0].binds.push_back(
		{ Input::WindowsKeyCode::E,
		std::function<void(float)>([&](float dt) { on_up(dt); }),
		Input::CallbackEvent::IsPressed }
	);
	keybind_set.categories[0].actions[0].binds.push_back(
		{ Input::WindowsKeyCode::LeftShift,
		std::function<void(float)>([&](float dt) { on_slow(dt); }),
		Input::CallbackEvent::IsPressed }
	);
	keybind_set.categories[0].actions[0].binds.push_back(
		{ Input::WindowsKeyCode::LeftShift,
		std::function<void(float)>([&](float dt) { on_not_slow(dt); }),
		Input::CallbackEvent::IsNotPressed }
	);
	keybind_set.categories[0].actions[0].binds.push_back(
		{ Input::WindowsKeyCode::RightMouseButton,
		std::function<void(float)>([&](float dt) { on_turn(dt); }),
		Input::CallbackEvent::IsPressed }
	);
}

void MyCamera::on_start() {
	add_component(camera);
	add_component(light);
}

void MyCamera::tick(float dt) {
	if (GetCursorPos(&current_cursor_pos)) {
		cursor_diff = { current_cursor_pos.x - previous_cursor_pos.x,
			current_cursor_pos.y - previous_cursor_pos.y };
	}
	previous_cursor_pos = current_cursor_pos;
}

void MyCamera::on_turn(float dt) {
	if (cursor_diff.x != 0 || cursor_diff.y != 0) {
		rotate(FVector3((float)cursor_diff.y / 7, (float)cursor_diff.x / 7, 0.0f));
	}
}

void MyCamera::on_forward(float dt) {
	translate(camera->direction_forward() * dt * current_speed);
}

void MyCamera::on_backward(float dt) {
	translate(camera->direction_backward() * dt * current_speed);
}

void MyCamera::on_left(float dt) {
	translate(camera->direction_left() * dt * current_speed);
}

void MyCamera::on_right(float dt) {
	translate(camera->direction_right() * dt * current_speed);
}

void MyCamera::on_up(float dt) {
	translate(FVector3(0.0f, 1.0f, 0.0f) * dt * current_speed);
}

void MyCamera::on_down(float dt) {
	translate(FVector3(0.0f, -1.0f, 0.0f) * dt * current_speed);
}

void MyCamera::on_slow(float dt) {
	current_speed = slow_speed;
}

void MyCamera::on_not_slow(float dt) {
	current_speed = regular_speed;
}

```

This example provides defines an object with a camera component that accepts user input for forward, backward, left, right, up, down, and slow.

Here, you can see that in the class's constructor, the input is configurated, components are defined, etc. In the on_start() overloaded function, the components are added to the object. It is important to know this; You can define components at any time, but you can not add the component to the object in the constructor.

Every component is different and serve their own purpose. You can find information on each component in the documentation.

Another important thing to notice is that this particular class's base class is NOT object! It's a Character! What's that?

A character is a subclass of an object, and it's only difference is that it has the capability to accept user input. This is possible through the KeybindSet class. You can learn more about user input in the documentation.

<h3>Getting started</h3>

Your LiquidEngine project begins in the main function. Inside of the main function, create an Engine object and pass in hInstance into the constructor. here, you can add objects into the active scene.

Once you're done setting up the world, compile the engine and let it run.

Here is an example main.cpp:

```
#include "Engine.h"
#include "Objects/MyCamera.h"
#include "Objects/Engine/DefaultCube.h"

int main() {
	if (FAILED(CoInitializeEx(nullptr, COINITBASE_MULTITHREADED))) { // If you are rendering textures, this must be added.
		OutputDebugStringA("Failed to initialize COM");
		return 1;
	}

	Engine engine(hInstance);

	engine.world.active_scene->add_character(std::make_shared<MyCamera>());

	std::shared_ptr<Object> sky = std::make_shared<Object>();
	engine.world.active_scene->add_object(sky);
	sky->add_component(std::make_shared<SkyComponent>());
	sky->get_component<SkyComponent>()->set_albedo_texture("Skybox.png");

	engine.world.active_scene->add_object(std::make_shared<DefaultCube>());

	engine.compile();
	engine.loop();
}

```

In this example, our camera character is added to the scene, as well as a sky, and a cube. LiquidEngine is then compiled and ran.
