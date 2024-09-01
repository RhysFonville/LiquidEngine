#include "PhysicsBody.h"

PhysicsBody::PhysicsBody() { }

void PhysicsBody::tick(float dt) noexcept {
	acceleration += (get_net_force() / get_mass()) * dt;
	angular_acceleration += (get_net_torque() / get_moment_of_inertia()) * dt;
	set_velocity(velocity+(acceleration*dt));

	std::erase_if(forces, [&](const Force &f){return f.type == Force::Type::Impulse;});
}

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
	if (frozen) return;
	this->velocity = velocity;
	recalculate_momentum();
}

void PhysicsBody::set_acceleration(const FVector3 &acceleration) {
	if (frozen) return;
	this->acceleration = acceleration;
}

void PhysicsBody::set_momentum(const FVector3 &momentum) {
	if (frozen) return;
	this->momentum = momentum;
	this->velocity = momentum / mass;
}

FVector3 PhysicsBody::get_net_torque() const noexcept {
	FVector3 net_torque;

	for (const Force &torque : torques) {
		net_torque += torque;
	}

	return net_torque;
}

float PhysicsBody::get_moment_of_inertia() const noexcept {
	return moment_of_inertia;
}

void PhysicsBody::set_moment_of_inertia(float moment_of_inertia) {
	if (moment_of_inertia != 0) {
		this->moment_of_inertia = moment_of_inertia;
		moment_of_inertia_tensor = moment_of_inertia * XMMatrixIdentity();
		moment_of_inertia_tensor_inverse = XMMatrixInverse(nullptr, moment_of_inertia_tensor);
	} else {
		throw std::exception("An object's moment of inertia must be set to >0.0f kilogram meters squared.");
	}
}

void PhysicsBody::set_angular_velocity(const FVector3 &angular_velocity) {
	this->angular_velocity = angular_velocity;
	recalculate_angular_momentum();
}

void PhysicsBody::set_angular_acceleration(const FVector3 &angular_acceleration) {
	this->angular_acceleration = angular_acceleration;
}

void PhysicsBody::set_angular_momentum(const FVector3 &angular_momentum) {
	this->angular_momentum = angular_momentum;
	this->angular_velocity = angular_momentum / moment_of_inertia;
}
