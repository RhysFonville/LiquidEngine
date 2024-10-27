#pragma once

#include "../Character.h"
#include "../Components/CameraComponent.h"
#include "../Components/PointLightComponent.h"

class MyCamera : public Character {
public:
	MyCamera();

	void pre_scene_compile() override;
	void tick(float dt) override;

	void on_turn(float dt);

	void on_forward(float dt);
	void on_backward(float dt);
	void on_left(float dt);
	void on_right(float dt);
	void on_up(float dt);
	void on_down(float dt);
	void on_slow(float dt);
	void on_not_slow(float dt);

	void on_increase_fov(float dt);
	void on_decrease_fov(float dt);

private:
	POINT current_cursor_pos = { 0, 0 };
	POINT cursor_diff = { 0, 0 };
	POINT previous_cursor_pos = current_cursor_pos;

	float regular_speed = 20.0f;
	float slow_speed = regular_speed / 2.0f;
	float current_speed = regular_speed;

	std::shared_ptr<CameraComponent> camera;
	std::shared_ptr<PointLightComponent> light;
};

