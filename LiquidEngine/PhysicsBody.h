#pragma once

#include "globalutil.h"

class PhysicsBody {
public:
	PhysicsBody();

	GET FVector3 get_net_force() const noexcept;
	GET float get_mass() const noexcept;

	void set_mass(float mass);

	void tick();

	FVector3 previous_position = FVector3();

	FVector3 velocity = FVector3();
	FVector3 acceleration = FVector3();
	FVector3 momentum = FVector3();

	std::vector<Mechanics::Force> forces;

private:
	float mass = 20.0f;
};

