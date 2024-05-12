#pragma once

#include "GraphicsComponent.h"
#include "SimpleMath.h"
#include "../Utility/globalutil.h"

/**
* Component for a camera.
*/
class CameraComponent : public GraphicsComponent {
public:
	CameraComponent();

	void set_position(FVector3 position) noexcept override;
	void set_rotation(FVector3 rotation) noexcept override;

	void rotate(FVector3 rotation) noexcept override;
	//void rotate_locally(FVector3 rotation) noexcept;

	GET FVector3 get_target() const noexcept;

	GET FVector3 direction_forward() const noexcept;
	GET FVector3 direction_backward() const noexcept;
	GET FVector3 direction_left() const noexcept;
	GET FVector3 direction_right() const noexcept;
	GET FVector3 direction_up() const noexcept;
	GET FVector3 direction_down() const noexcept;

	GET FVector3 direction_real_up() const noexcept;

	GET XMMATRIX get_wvp() const noexcept;

	GET float get_fov() const noexcept;
	void set_fov(float fov) noexcept;

	void update(const FVector2 &size) noexcept;

	bool operator==(const CameraComponent &component) const noexcept;
	void operator=(const CameraComponent &component) noexcept;

	static const Type component_type = Type::CameraComponent;

protected:
	void render_editor_gui_section() override;

private:
	friend class Renderer;

	void set_view() noexcept;

	float fov = 90.0f;

	FVector3 target = { 0.0f, 0.0f, 1.0f };
	FVector3 up = { 0.0f, 1.0f, 0.0f };

	FVector3 forward = global_forward;
	FVector3 backward = global_backward;
	FVector3 left = global_left;
	FVector3 right = global_right;
	FVector3 down = global_down;

	XMMATRIX WVP = XMMatrixIdentity();
	XMMATRIX world = XMMatrixIdentity();
	XMMATRIX view = XMMatrixIdentity();
	XMMATRIX projection = XMMatrixIdentity();

	static constexpr float pi_div_180 = 0.01745329251f;
};

