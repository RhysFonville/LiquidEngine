#pragma once

#include "../Character.h"
#include "../Components/CameraComponent.h"
#include "../Components/PointLightComponent.h"

class MyCamera : public Character {
public:
	MyCamera();

	void on_start() override;
	void tick(float dt) override;

private:
	POINT previous_cursor_pos = { 0, 0 };
	int wait = 0;

	float speed = 6.0f;

	std::shared_ptr<CameraComponent> camera;
	std::shared_ptr<PointLightComponent> light;
};

