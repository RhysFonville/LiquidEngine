#include "CameraComponent.h"

CameraComponent::CameraComponent()
	: GraphicsComponent{} { }

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

		rotation.x = degrees_to_radians(transform.rotation.x);
		rotation.y = degrees_to_radians(transform.rotation.y);
		rotation.z = degrees_to_radians(transform.rotation.z);

		XMMATRIX rot_matrix = XMMatrixRotationRollPitchYaw(rotation.x, rotation.y, rotation.z);

		target = XMVector3TransformCoord(global_forward.to_xmvec(),
			rot_matrix
		);
		XMVector3Normalize(target.to_xmvec());

		up = XMVector3TransformCoord(global_up.to_xmvec(),
			rot_matrix
		);
		XMVector3Normalize(up.to_xmvec());

		target += transform.position;

		forward = XMVector3TransformCoord(global_forward.to_xmvec(), rot_matrix);
		backward = XMVector3TransformCoord(global_backward.to_xmvec(), rot_matrix);
		left = XMVector3TransformCoord(global_left.to_xmvec(), rot_matrix);
		right = XMVector3TransformCoord(global_right.to_xmvec(), rot_matrix);

		XMMATRIX roll_matrix = XMMatrixRotationZ(rotation.z);

		down = XMVector3TransformCoord(global_up.to_xmvec(), roll_matrix);

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

float CameraComponent::get_fov() const noexcept {
	return fov;
}

void CameraComponent::set_fov(float fov) noexcept {
	this->fov = fov;
	changed = true;
}

void CameraComponent::update(const FVector2 &size) noexcept {
	if (perspective)
		projection = XMMatrixPerspectiveFovLH(fov*pi_div_180, size.x/size.y, near_plane, far_plane);
	else
		projection = XMMatrixOrthographicLH(size.x/fov, size.y/fov, near_plane, far_plane);

	world = XMMatrixIdentity();
	WVP = world * view * projection;
}

void CameraComponent::set_view() noexcept {
	view = XMMatrixLookAtLH(transform.position.to_xmvec(), this->target.to_xmvec(), this->up.to_xmvec());
	changed = true;
}

float CameraComponent::get_near_plane() const noexcept {
	return near_plane;
}

void CameraComponent::set_near_plane(float near_plane) noexcept {
	this->near_plane = near_plane;
	changed = true;
}

float CameraComponent::get_far_plane() const noexcept {
	return far_plane;
}

void CameraComponent::set_far_plane(float far_plane) noexcept {
	this->far_plane = far_plane;
	changed = true;
}

bool CameraComponent::is_perspective() const noexcept {
	return perspective;
}

void CameraComponent::is_perspective(bool perspective) noexcept {
	this->perspective = perspective;
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

void CameraComponent::render_editor_gui_section() {
	float fov{this->fov};
	if (ImGui::InputFloat("Field of view", &fov))
		set_fov(fov);

	bool perspective{this->perspective};
	if (ImGui::Checkbox("Perspective projection", &perspective))
		is_perspective(perspective);

	float near_plane{this->near_plane};
	if (ImGui::InputFloat("Near plane", &near_plane))
		set_near_plane(near_plane);

	float far_plane{this->far_plane};
	if (ImGui::InputFloat("Far plane", &far_plane))
		set_far_plane(far_plane);
}
