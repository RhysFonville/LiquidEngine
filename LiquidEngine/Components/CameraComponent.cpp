#include "CameraComponent.h"

CameraComponent::CameraComponent()
	: GraphicsComponent(Type::CameraComponent) { }

void CameraComponent::set_position(FVector3 position) noexcept {
	FVector3 difference = position - transform.position;

	if (!difference.is_zero()) {
		transform.position = position;
		target += difference;
	}

	set_view();
}

void CameraComponent::set_rotation(FVector3 rotation) noexcept {
	rotate(rotation - transform.rotation);
}

void CameraComponent::rotate(FVector3 rotation) noexcept {
	if (!rotation.is_zero()) {
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

//void CameraComponent::rotate_locally(FVector3 rotation) noexcept {
//	transform.rotation += rotation;
//
//	rotation.x = degrees_to_theta(transform.rotation.x);
//	rotation.y = degrees_to_theta(transform.rotation.y);
//	rotation.z = degrees_to_theta(transform.rotation.z);
//
//	XMMATRIX rot_matrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);
//
//	target = XMVector3TransformCoord(forward,
//		rot_matrix
//	);
//	XMVector3Normalize(target);
//
//	up = XMVector3TransformCoord(up,
//		rot_matrix
//	);
//	XMVector3Normalize(up);
//
//	target += transform.position;
//
//	forward = XMVector3TransformCoord(global_forward, rot_matrix);
//	backward = XMVector3TransformCoord(global_backward, rot_matrix);
//	left = XMVector3TransformCoord(global_left, rot_matrix);
//	right = XMVector3TransformCoord(global_right, rot_matrix);
//
//	XMMATRIX roll_matrix = XMMatrixRotationZ(rotation.z);
//
//	down = XMVector3TransformCoord(global_up, roll_matrix);
//
//	set_view();
//}

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

XMMATRIX CameraComponent::get_wvp() const noexcept {
	return WVP;
}

void CameraComponent::update(const FVector2 &size) noexcept {
	projection = XMMatrixPerspectiveFovLH( 0.4f*3.14f, size.x/size.y, 0.05f, 1000.0f);
	world = XMMatrixIdentity();
	WVP = world * view * projection;
}

void CameraComponent::set_view() noexcept {
	view = XMMatrixLookAtLH((XMVECTOR)transform.position, (XMVECTOR)this->target, (XMVECTOR)this->up);
	changed = true;
}

bool CameraComponent::operator==(const CameraComponent &component) const noexcept {
	return
		((Component*)this == (Component*)&component &&
		target == component.target &&
		up == component.up &&
		forward == component.forward &&
		backward == component.backward &&
		left == component.left &&
		right == component.right &&
		down == component.down/* &&
		WVP == component.WVP &&
		world == component.world &&
		view == component.view &&
		projection == component.projection*/);
}

void CameraComponent::operator=(const CameraComponent &component) noexcept {
	target = component.target;
	up = component.up;
	forward = component.forward;
	backward = component.backward;
	left = component.left;
	right = component.right;
	down = component.down;
	WVP = component.WVP;
	world = component.world;
	view = component.view;
	projection = component.projection;
}
