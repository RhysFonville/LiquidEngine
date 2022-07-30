#include "CameraComponent.h"

CameraComponent::CameraComponent(const FPosition3 &position, const FRotation3 &rotation)
	: Component(Type::CameraComponent, Transform()) {
	translate(position);
	rotate(rotation);
	set_view();
}

void CameraComponent::set_position(FPosition3 position) noexcept {
	FPosition3 difference = position - transform.position;

	if (!difference.is_zero()) {
		transform.position = position;
		target += difference;
	}

	set_view();
}

void CameraComponent::set_rotation(FRotation3 rotation) noexcept {
	rotate(rotation - transform.rotation);
}

void CameraComponent::rotate(FRotation3 rotation) noexcept {
	if (!rotation.is_zero()) {
		/*transform.rotation += rotation;

		rotation.x = degrees_to_theta(rotation.x);
		rotation.y = degrees_to_theta(rotation.y);
		rotation.z = degrees_to_theta(rotation.z);

		// Rotation x = Target y & z
		// Rotation y = Target x & z
		// Rotation z = Up

		FVector3 rx = {
		cosf(rotation.x)*cosf(rotation.y),
		cosf(rotation.x)*sinf(rotation.y)*sinf(rotation.z)-sinf(rotation.x)*cosf(rotation.z),
		cosf(rotation.x)*sinf(rotation.y)*cosf(rotation.z)+sinf(rotation.x)*sinf(rotation.z)
		};
		FVector3 ry = {
		sinf(rotation.x)*cosf(rotation.y),
		sinf(rotation.x)*sinf(rotation.y)*sinf(rotation.z)+cosf(rotation.x)*cosf(rotation.z),
		sinf(rotation.x)*sinf(rotation.y)*cosf(rotation.z)-cosf(rotation.x)*sinf(rotation.z)
		};
		FVector3 rz = {
		-sinf(rotation.y),
		cosf(rotation.y)*sinf(rotation.z),
		cosf(rotation.y)*cosf(rotation.z)
		};

		float xpt = rx * target_from_origin;
		float ypt = ry * target_from_origin;
		float zpt = rz * target_from_origin;

		target_from_origin.x = xpt;
		target_from_origin.y = ypt;
		target_from_origin.z = zpt;

		target.x = xpt;
		target.y = ypt;
		target.z = zpt;

		target += transform.position;

		float xpu = rx * up_from_origin;
		float ypu = ry * up_from_origin;
		float zpu = rz * up_from_origin;

		up_from_origin.x = xpu;
		up_from_origin.y = ypu;
		up_from_origin.z = zpu;

		up.x = xpu;
		up.y = ypu;
		up.z = zpu;*/

		/*rotation.x = degrees_to_theta(rotation.x);
		rotation.y = degrees_to_theta(rotation.y);
		rotation.z = degrees_to_theta(rotation.z);

		FVector3 rx = {
			cosf(rotation.x)*cosf(rotation.y),
			cosf(rotation.x)*sinf(rotation.y)*sinf(rotation.z)-sinf(rotation.x)*cosf(rotation.z),
			cosf(rotation.x)*sinf(rotation.y)*cosf(rotation.z)+sinf(rotation.x)*sinf(rotation.z)
		};
		FVector3 ry = {
			sinf(rotation.x)*cosf(rotation.y),
			sinf(rotation.x)*sinf(rotation.y)*sinf(rotation.z)+cosf(rotation.x)*cosf(rotation.z),
			sinf(rotation.x)*sinf(rotation.y)*cosf(rotation.z)-cosf(rotation.x)*sinf(rotation.z)
		};
		FVector3 rz = {
			-sinf(rotation.y),
			cosf(rotation.y)*sinf(rotation.z),
			cosf(rotation.y)*cosf(rotation.z)
		};

		target = Transformations::rotate_origin(rx, ry, rz, rotation, target_from_origin);
		target += transform.position;
		up = Transformations::rotate_origin(rx, ry, rz, rotation, up_from_origin);*/
		
		transform.rotation += rotation;

		rotation.x = degrees_to_theta(transform.rotation.x);
		rotation.y = degrees_to_theta(transform.rotation.y);
		rotation.z = degrees_to_theta(transform.rotation.z);

		XMMATRIX rot_matrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

		target = XMVector3TransformCoord(global_forward,
			rot_matrix
		);
		XMVector3Normalize(target);

		up = XMVector3TransformCoord(global_up,
			rot_matrix
		);
		XMVector3Normalize(up);

		target += transform.position;

		forward = XMVector3TransformCoord(global_forward, rot_matrix);
		backward = XMVector3TransformCoord(global_backward, rot_matrix);
		left = XMVector3TransformCoord(global_left, rot_matrix);
		right = XMVector3TransformCoord(global_right, rot_matrix);

		XMMATRIX roll_matrix = XMMatrixRotationZ(rotation.z);

		down = XMVector3TransformCoord(global_up, roll_matrix);

		set_view();
	}
}

void CameraComponent::rotate_locally(FRotation3 rotation) noexcept {
	transform.rotation += rotation;

	rotation.x = degrees_to_theta(transform.rotation.x);
	rotation.y = degrees_to_theta(transform.rotation.y);
	rotation.z = degrees_to_theta(transform.rotation.z);

	XMMATRIX rot_matrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

	target = XMVector3TransformCoord(forward,
		rot_matrix
	);
	XMVector3Normalize(target);

	up = XMVector3TransformCoord(up,
		rot_matrix
	);
	XMVector3Normalize(up);

	target += transform.position;

	forward = XMVector3TransformCoord(global_forward, rot_matrix);
	backward = XMVector3TransformCoord(global_backward, rot_matrix);
	left = XMVector3TransformCoord(global_left, rot_matrix);
	right = XMVector3TransformCoord(global_right, rot_matrix);

	XMMATRIX roll_matrix = XMMatrixRotationZ(rotation.z);

	down = XMVector3TransformCoord(global_up, roll_matrix);

	set_view();
}

FVector3 CameraComponent::get_target() const noexcept {
	return target;
}

FVector3 CameraComponent::direction_forward() const noexcept {
	return forward;
}

FVector3 CameraComponent::direction_backward() const noexcept {
	return backward;
}

FVector3 CameraComponent::direction_left() const noexcept {
	return left;
}

FVector3 CameraComponent::direction_right() const noexcept {
	return right;
}

FVector3 CameraComponent::direction_up() const noexcept {
	return up;
}

FVector3 CameraComponent::direction_down() const noexcept {
	return down;
}

FVector3 CameraComponent::direction_real_up() const noexcept {
	return up;
}

void CameraComponent::update(const Size2 &size) noexcept {
	projection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, (float)size.x/size.y, 0.05f, 1000.0f);
	world = XMMatrixIdentity();
	WVP = world * view * projection;
}

void CameraComponent::set_view() noexcept {
	view = XMMatrixLookAtLH((XMVECTOR)transform.position, (XMVECTOR)this->target, (XMVECTOR)this->up);
}
