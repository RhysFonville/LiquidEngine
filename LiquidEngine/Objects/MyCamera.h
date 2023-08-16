#pragma once

#include "../Object.h"
#include "../Components/CameraComponent.h"
#include "../Components/PointLightComponent.h"

class MyCamera : public Object {
public:
	MyCamera();

	void on_start() override;
	void tick(float dt) override;

private:
	POINT previous_cursor_pos = { 0, 0 };
	int wait = 0;

	float speed = 3.0f;

	std::shared_ptr<CameraComponent> camera;
	std::shared_ptr<PointLightComponent> light;
};

