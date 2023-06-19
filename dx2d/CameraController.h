#pragma once

#include "ObjectBehavior.h"
#include "CameraComponent.h"

class CameraController : public ObjectBehavior {
public:
	CameraController(Object* object);

	void tick(float dt) override;

private:
	POINT previous_cursor_pos = { 0, 0 };
};
