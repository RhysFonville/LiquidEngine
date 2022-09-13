#include "CameraController.h"

CameraController::CameraController(const ObjectVector &objects, std::shared_ptr<Object> &object)
	: ObjectBehavior(objects, object, "CameraController") { }

void CameraController::tick() {
	CameraComponent &camera = *(object->get_component<CameraComponent>());

	if (GetKeyState(0x57) & 0x8000) { // W
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(camera.direction_forward() / 20.0f);
		} else {
			object->translate(camera.direction_forward() / 5.0f);
		}
	}
	if (GetKeyState(0x41) & 0x8000) { // A
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(camera.direction_left() / 20.0f);
		} else {
			object->translate(camera.direction_left() / 5.0f);
		}
	}
	if (GetKeyState(0x53) & 0x8000) { // S
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(camera.direction_backward() / 20.0f);
		} else {
			object->translate(camera.direction_backward() / 5.0f);
		}
	}
	if (GetKeyState(0x44) & 0x8000) { // D
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(camera.direction_right() / 20.0f);
		} else {
			object->translate(camera.direction_right() / 5.0f);
		}
	}
	if (GetKeyState(0x51) & 0x8000) { // Q
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(0.0f, -1.0f, 0.0f) / 20.0f);
		} else {
			object->translate(FVector3(0.0f, -1.0f, 0.0f) / 5.0f);
		}
	}
	if (GetKeyState(0x45) & 0x8000) { // E
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(0.0f, 1.0f, 0.0f) / 20.0f);
		} else {
			object->translate(FVector3(0.0f, 1.0f, 0.0f) / 5.0f);
		}
	}

	POINT current_cursor_pos;
	if (GetCursorPos(&current_cursor_pos)) {
		if (GetKeyState(0x02) & 0x8000) { // Right Mouse Button
			if (previous_cursor_pos.x != 0 && previous_cursor_pos.y != 0) {
				POINT diff = { current_cursor_pos.x - previous_cursor_pos.x,
							   current_cursor_pos.y - previous_cursor_pos.y };
				object->rotate(FRotation3((float)diff.y / 7, (float)diff.x / 7, 0.0f));
			}
		}
	}
	previous_cursor_pos = current_cursor_pos;
}
