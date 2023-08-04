#pragma once

class PhysicsBody {
public:
	PhysicsBody();

	FVector3 previous_position = FVector3();

	FVector3 velocity = FVector3();
	FVector3 acceleration = FVector3();
	float speed = 0.0f; 
	float kinetic_energy = 0.0f;
	FVector3 momentum = FVector3();

	std::vector<Mechanics::Force> forces;

	GET FVector3 get_net_force() const noexcept {
		FVector3 net_force;

		for (const Mechanics::Force &force : forces) {
			net_force += force;
		}

		return net_force;
	}

	GET float get_mass() const noexcept {
		return mass;
	}

	void set_mass(float mass) {
		if (mass != 0) {
			this->mass = mass;
		} else {
			throw std::exception("An object's mass must be set to >0.0f kilograms.");
		}
	}

private:
	float mass = 20.0f;
};

