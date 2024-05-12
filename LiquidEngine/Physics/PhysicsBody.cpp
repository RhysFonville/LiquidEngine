#include "PhysicsBody.h"

PhysicsBody::PhysicsBody() { }

FVector3 PhysicsBody::get_net_force() const noexcept {
	FVector3 net_force;

	for (const Force &force : forces) {
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

void PhysicsBody::set_velocity(const FVector3 &velocity) {
	this->velocity = velocity;
	recalculate_momentum();
}

void PhysicsBody::set_acceleration(const FVector3 &acceleration) {
	this->acceleration = acceleration;
}

void PhysicsBody::set_momentum(const FVector3 &momentum) {
	this->momentum = momentum;
	this->velocity = momentum / mass;
}

void PhysicsBody::tick(float dt) noexcept {
	acceleration += (get_net_force() / get_mass()) * dt;
	set_velocity(velocity+(acceleration*dt));
}
