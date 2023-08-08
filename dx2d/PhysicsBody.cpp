#include "PhysicsBody.h"

PhysicsBody::PhysicsBody() { }

FVector3 PhysicsBody::get_net_force() const noexcept {
	FVector3 net_force;

	for (const Mechanics::Force &force : forces) {
		net_force += force;
	}

	return net_force;
}

float PhysicsBody::get_mass() const noexcept {
	return mass;
}

void PhysicsBody::set_mass(float mass) {
	if (mass != 0) {
		this->mass = mass;
	} else {
		throw std::exception("An object's mass must be set to >0.0f kilograms.");
	}
}
