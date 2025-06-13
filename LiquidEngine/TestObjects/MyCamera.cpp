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
	keybind_set.categories[0].actions[0].binds.push_back(
		{ Input::WindowsKeyCode::OEMPlus,
		std::function<void(float)>([&](float dt) { on_increase_fov(dt); }),
		Input::CallbackEvent::IsPressed }
	);
	keybind_set.categories[0].actions[0].binds.push_back(
		{ Input::WindowsKeyCode::OEMMinus,
		std::function<void(float)>([&](float dt) { on_decrease_fov(dt); }),
		Input::CallbackEvent::IsPressed }
	);
}

void MyCamera::pre_scene_compile() {
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
		rotate(FVector3{(float)cursor_diff.y / 7.0f, (float)cursor_diff.x / 7.0f, 0.0f});
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

void MyCamera::on_increase_fov(float dt) {
	camera->set_fov(camera->get_fov()+15.0f*dt);
}

void MyCamera::on_decrease_fov(float dt) {
	camera->set_fov(camera->get_fov()-15.0f*dt);
}

void MyCamera::render_editor_gui_section() {
	ImGui::InputFloat("Regular speed", &regular_speed);
	ImGui::InputFloat("Slow speed", &slow_speed);
}
