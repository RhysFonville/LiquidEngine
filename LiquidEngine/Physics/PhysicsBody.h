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

	FVector3 previous_position = FVector3();
	std::vector<Force> forces;

private:
	float mass = 20.0f;

	FVector3 velocity{};
	FVector3 acceleration{};
	FVector3 momentum{};

	void recalculate_momentum() { momentum = mass * (velocity); }
};

