#include "MyCamera.h"

MyCamera::MyCamera() : Object() {
	root_component.add_component(CameraComponent());
}

void MyCamera::tick(float dt) {
	if (GetKeyState(0x57) & 0x8000) { // W
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(camera.direction_forward() * dt / 2.0f);
		} else {
			translate(camera.direction_forward() * dt);
		}
	}
	if (GetKeyState(0x41) & 0x8000) { // A
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(camera.direction_left() * dt / 2.0f);
		} else {
			translate(camera.direction_left() * dt);
		}
	}
	if (GetKeyState(0x53) & 0x8000) { // S
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(camera.direction_backward() * dt / 2.0f);
		} else {
			translate(camera.direction_backward() * dt);
		}
	}
	if (GetKeyState(0x44) & 0x8000) { // D
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(camera.direction_right() * dt / 2.0f);
		} else {
			translate(camera.direction_right() * dt);
		}
	}
	if (GetKeyState(0x51) & 0x8000) { // Q
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(FVector3(0.0f, -1.0f, 0.0f) * dt / 2.0f);
		} else {
			translate(FVector3(0.0f, -1.0f, 0.0f) * dt);
		}
	}
	if (GetKeyState(0x45) & 0x8000) { // E
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			translate(FVector3(0.0f, 1.0f, 0.0f) * dt / 2.0f);
		} else {
			translate(FVector3(0.0f, 1.0f, 0.0f) * dt);
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
				light.data.diffuse.r--;
			} else {
				light.data.diffuse.r++;
			}
			wait = 0;
		} else {
			wait++;
		}
	}
	if (GetKeyState(0x47) & 0x8000) { // G
		if (wait == 100) {
			if (GetKeyState(VK_LSHIFT) & 0x8000) {
				light.data.diffuse.g--;
			} else {
				light.data.diffuse.g++;
			}
			wait = 0;
		} else {
			wait++;
		}
	}
	if (GetKeyState(0x42) & 0x8000) { // B
		if (wait == 100) {
			if (GetKeyState(VK_LSHIFT) & 0x8000) {
				light.data.diffuse.b--;
			} else {
				light.data.diffuse.b++;
			}
			wait = 0;
		} else {
			wait++;
		}
	}
}
