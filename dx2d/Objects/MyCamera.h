#pragma once

#include "../Object.h"
#include "../Components/CameraComponent.h"
#include "../Components/PointLightComponent.h"

class MyCamera : public Object {
public:
	MyCamera() { }

	void on_start() override;
	void tick(float dt) override;

private:
	POINT previous_cursor_pos = { 0, 0 };
	int wait = 0;

	CameraComponent camera;
	PointLightComponent light;
};

