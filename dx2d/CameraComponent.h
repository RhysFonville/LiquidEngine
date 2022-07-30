#pragma once

#include "Component.h"
#include "SimpleMath.h"

class CameraComponent : public Component {
public:
	CameraComponent(const FPosition3 &position = FPosition3(),
		const FRotation3 &rotation = FRotation3());

	void set_position(FPosition3 position) noexcept override;
	void set_rotation(FRotation3 rotation) noexcept override;

	void rotate(FRotation3 rotation) noexcept override;
	void rotate_locally(FRotation3 rotation) noexcept;

	GET FVector3 get_target() const noexcept;

	GET FVector3 direction_forward() const noexcept;
	GET FVector3 direction_backward() const noexcept;
	GET FVector3 direction_left() const noexcept;
	GET FVector3 direction_right() const noexcept;
	GET FVector3 direction_up() const noexcept;
	GET FVector3 direction_down() const noexcept;

	GET FVector3 direction_real_up() const noexcept;

	void update(const Size2 &size) noexcept;

	static const Type component_type = Type::CameraComponent;

private:
	friend class GraphicsScene;

	void set_view() noexcept;

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
};