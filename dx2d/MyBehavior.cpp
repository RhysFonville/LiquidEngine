#include "MyBehavior.h"

MyBehavior::MyBehavior(Object* object) : ObjectBehavior(object) { }

void MyBehavior::tick(float dt) {
	if (GetKeyState(VK_UP) & 0x8000) { // W
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(0.0f, 0.0f, 1.0f) * dt / 2.0f);
		} else {
			object->translate(FVector3(0.0f, 0.0f, 1.0f) * dt);
		}
	}
	if (GetKeyState(VK_LEFT) & 0x8000) { // A
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(-1.0f, 0.0f, 0.0f) * dt / 2.0f);
		} else {
			object->translate(FVector3(-1.0f, 0.0f, 0.0f) * dt);
		}
	}
	if (GetKeyState(VK_DOWN) & 0x8000) { // S
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(0.0f, 0.0f, -1.0f) * dt / 2.0f);
		} else {
			object->translate(FVector3(0.0f, 0.0f, -1.0f) * dt);
		}
	}
	if (GetKeyState(VK_RIGHT) & 0x8000) { // D
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(1.0f, 0.0f, 0.0f) * dt / 2.0f);
		} else {
			object->translate(FVector3(1.0f, 0.0f, 0.0f) * dt);
		}
	}
	if (GetKeyState(VK_NUMPAD1) & 0x8000) { // Q
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(0.0f, -1.0f, 0.0f) * dt / 2.0f);
		} else {
			object->translate(FVector3(0.0f, -1.0f, 0.0f) * dt);
		}
	}
	if (GetKeyState(VK_NUMPAD2) & 0x8000) { // E
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(0.0f, 1.0f, 0.0f) * dt / 2.0f);
		} else {
			object->translate(FVector3(0.0f, 1.0f, 0.0f) * dt);
		}
	}
}
