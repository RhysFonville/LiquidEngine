#include "MyBehavior.h"

MyBehavior::MyBehavior(const ObjectVector &objects, std::shared_ptr<Object> &object)
	: ObjectBehavior(objects, object, "MyBehavior") { }

void MyBehavior::tick() {
	if (GetKeyState(VK_UP) & 0x8000) { // W
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(0.0f, 0.0f, 1.0f) / 20.0f);
		} else {
			object->translate(FVector3(0.0f, 0.0f, 1.0f) / 5.0f);
		}
	}
	if (GetKeyState(VK_LEFT) & 0x8000) { // A
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(-1.0f, 0.0f, 0.0f) / 20.0f);
		} else {
			object->translate(FVector3(-1.0f, 0.0f, 0.0f) / 5.0f);
		}
	}
	if (GetKeyState(VK_DOWN) & 0x8000) { // S
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(0.0f, 0.0f, -1.0f) / 20.0f);
		} else {
			object->translate(FVector3(0.0f, 0.0f, -1.0f) / 5.0f);
		}
	}
	if (GetKeyState(VK_RIGHT) & 0x8000) { // D
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(1.0f, 0.0f, 0.0f) / 20.0f);
		} else {
			object->translate(FVector3(1.0f, 0.0f, 0.0f) / 5.0f);
		}
	}
	if (GetKeyState(VK_NUMPAD1) & 0x8000) { // Q
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(0.0f, -1.0f, 0.0f) / 20.0f);
		} else {
			object->translate(FVector3(0.0f, -1.0f, 0.0f) / 5.0f);
		}
	}
	if (GetKeyState(VK_NUMPAD2) & 0x8000) { // E
		if (GetKeyState(VK_LSHIFT) & 0x8000) {
			object->translate(FVector3(0.0f, 1.0f, 0.0f) / 20.0f);
		} else {
			object->translate(FVector3(0.0f, 1.0f, 0.0f) / 5.0f);
		}
	}
}
