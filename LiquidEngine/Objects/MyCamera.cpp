#include "MyCamera.h"

MyCamera::MyCamera()
	: camera(std::make_shared<CameraComponent>()),
	light(std::make_shared<PointLightComponent>()) { }

void MyCamera::on_start() {
	add_component(camera);
	add_component(light);
}

void MyCamera::tick(float dt) {
	if (GetKeyState(0x57) & 0x8000) { // W
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(camera->direction_forward() * dt * speed / 2.0f);
		} else {
			translate(camera->direction_forward() * dt * speed);
		}
	}
	if (GetKeyState(0x41) & 0x8000) { // A
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(camera->direction_left() * dt * speed / 2.0f);
		} else {
			translate(camera->direction_left() * dt * speed);
		}
	}
	if (GetKeyState(0x53) & 0x8000) { // S
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(camera->direction_backward() * dt * speed / 2.0f);
		} else {
			translate(camera->direction_backward() * dt * speed);
		}
	}
	if (GetKeyState(0x44) & 0x8000) { // D
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(camera->direction_right() * dt * speed / 2.0f);
		} else {
			translate(camera->direction_right() * dt * speed);
		}
	}
	if (GetKeyState(0x51) & 0x8000) { // Q
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(FVector3(0.0f, -1.0f, 0.0f) * dt * speed / 2.0f);
		} else {
			translate(FVector3(0.0f, -1.0f, 0.0f) * dt * speed);
		}
	}
	if (GetKeyState(0x45) & 0x8000) { // E
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(FVector3(0.0f, 1.0f, 0.0f) * dt * speed / 2.0f);
		} else {
			translate(FVector3(0.0f, 1.0f, 0.0f) * dt * speed);
		}
	}

	POINT current_cursor_pos;
	if (GetCursorPos(&current_cursor_pos)) {
		if (GetKeyState(0x02) & 0x8000) { // Right Mouse Button
			if (previous_cursor_pos.x != 0 && previous_cursor_pos.y != 0) {
				POINT diff = { current_cursor_pos.x - previous_cursor_pos.x,
							   current_cursor_pos.y - previous_cursor_pos.y };
				rotate(FVector3((float)diff.y / 7, (float)diff.x / 7, 0.0f));
			}
		}
	}
	previous_cursor_pos = current_cursor_pos;

	if (GetKeyState(0x52) & 0x8000) { // R
		if (wait == 100) {
			if (GetKeyState(VK_LSHIFT) & 0x8000) {
				light->albedo.r--;
			} else {
				light->albedo.r++;
			}
			wait = 0;
		} else {
			wait++;
		}
	}
	if (GetKeyState(0x47) & 0x8000) { // G
		if (wait == 100) {
			if (GetKeyState(VK_LSHIFT) & 0x8000) {
				light->albedo.g--;
			} else {
				light->albedo.g++;
			}
			wait = 0;
		} else {
			wait++;
		}
	}
	if (GetKeyState(0x42) & 0x8000) { // B
		if (wait == 100) {
			if (GetKeyState(VK_LSHIFT) & 0x8000) {
				light->albedo.b--;
			} else {
				light->albedo.b++;
			}
			wait = 0;
		} else {
			wait++;
		}
	}
}
