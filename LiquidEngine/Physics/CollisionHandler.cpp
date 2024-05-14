#include "CollisionHandler.h"

void CollisionHandler::handle_collision(const FVector3 &pos1, PhysicsBody &obj1, const FVector3 &pos2, PhysicsBody &obj2) noexcept {
	constexpr float cor = 1.0f;
	
	FVector3 v1 = obj1.get_velocity();
	FVector3 v2 = obj2.get_velocity();
	float m1 = obj1.get_mass();
	float m2 = obj2.get_mass();

	FVector3 n = (pos2 - pos1) / FVector3{pos2 - pos1}.magnitude();
	float meff = 1.0f / ((1.0f/m1) + (1.0f/m2));
	float vimp = dot(n, v1-v2);
	float J = (1.0f+cor)*meff*vimp;
	FVector3 dv1 = -(J/m1)*n;
	FVector3 dv2 = (J/m2)*n;

	FVector3 vf1 = v1+dv1;
	FVector3 vf2 = v2+dv2;

	obj1.set_velocity(vf1);
	obj2.set_velocity(vf2);
}