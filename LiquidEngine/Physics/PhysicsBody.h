#pragma once

#include <vector>
#include "physicsutil.h"

class PhysicsBody {
public:
	PhysicsBody();

	void tick(float dt) noexcept;

	GET FVector3 get_net_force() const noexcept;

	GET float get_mass() const noexcept;
	void set_mass(float mass);

	GET FVector3 get_velocity() const noexcept { return velocity; }
	void set_velocity(const FVector3 &velocity);

	GET FVector3 get_acceleration() const noexcept { return acceleration; }
	void set_acceleration(const FVector3 &acceleration);

	GET FVector3 get_momentum() const noexcept { return momentum; }
	void set_momentum(const FVector3 &momentum);

	GET FVector3 get_net_torque() const noexcept;

	GET float get_moment_of_inertia() const noexcept;
	void set_moment_of_inertia(float moment_of_inertia);

	GET XMMATRIX get_moment_of_inertia_tensor() const noexcept { return moment_of_inertia_tensor; }
	GET XMMATRIX get_moment_of_inertia_tensor_inverse() const noexcept { return moment_of_inertia_tensor_inverse; }

	GET FVector3 get_angular_velocity() const noexcept { return angular_velocity; }
	void set_angular_velocity(const FVector3 &velocity);

	GET FVector3 get_angular_acceleration() const noexcept { return angular_acceleration; }
	void set_angular_acceleration(const FVector3 &acceleration);

	GET FVector3 get_angular_momentum() const noexcept { return angular_momentum; }
	void set_angular_momentum(const FVector3 &momentum);

	FVector3 previous_position{};
	std::vector<Force> forces{};
	std::vector<Force> torques{};

	bool frozen{false};

private:
	float mass{1.0f};
	float moment_of_inertia{mass};
	XMMATRIX moment_of_inertia_tensor{moment_of_inertia * XMMatrixIdentity()};
	XMMATRIX moment_of_inertia_tensor_inverse{XMMatrixInverse(nullptr, moment_of_inertia_tensor)};

	FVector3 velocity{};
	FVector3 acceleration{};
	FVector3 momentum{};

	FVector3 angular_velocity{};
	FVector3 angular_acceleration{};
	FVector3 angular_momentum{};

	void recalculate_momentum() { momentum = mass * velocity; }
	void recalculate_angular_momentum() { angular_momentum = moment_of_inertia * angular_velocity; }
};

