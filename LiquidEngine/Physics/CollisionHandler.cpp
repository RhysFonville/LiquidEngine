#include "CollisionHandler.h"

void CollisionHandler::handle_collision(const OverlapEventInfo &info) noexcept {
	static constexpr float e = 1.0f;

	auto &obj1 = info.objects.first.component;
	auto &obj2 = info.objects.second.component;

	FVector3 v1 = obj1->physics_body.get_velocity();
	FVector3 v2 = obj2->physics_body.get_velocity();
	float m1 = obj1->physics_body.get_mass();
	float m2 = obj2->physics_body.get_mass();

	FVector3 w1 = obj1->physics_body.get_angular_velocity();
	FVector3 w2 = obj2->physics_body.get_angular_velocity();

	FVector3 r1 = info.overlap_info.point_of_overlap - obj1->get_position();
	FVector3 r2 = info.overlap_info.point_of_overlap - obj2->get_position();

	XMMATRIX I1 = obj1->physics_body.get_moment_of_inertia_tensor();
	XMMATRIX I2 = obj2->physics_body.get_moment_of_inertia_tensor();

	XMMATRIX I1inv = obj1->physics_body.get_moment_of_inertia_tensor_inverse();
	XMMATRIX I2inv = obj2->physics_body.get_moment_of_inertia_tensor_inverse();

	//FVector3 vrel = (v2 + w2*r2) + (v1 + w1*r1);
	FVector3 v12 = v1 - v2;
	
	FVector3 n = (obj2->get_position() - obj1->get_position()) / FVector3{obj2->get_position() - obj1->get_position()}.magnitude();
	//float meff = 1.0f / ((1.0f/m1) + (1.0f/m2));
	//float vimp = dot(n, v1-v2);
	//float J = (1.0f+e)*meff*vimp;
	float Jnum = -(1.0f + e) * dot(v12, n);
	float J1 = powf(n.magnitude(), 2) * (1.0f/m1 + 1.0f/m2);
	FVector3 J2 = cross(I1inv*cross(r1, n), r1);
	FVector3 J3 = cross(I2inv*cross(r2, n), r2);
	float Jden = (J1 + dot(J2 + J3, n));
	float J = Jnum / Jden;
	
	FVector3 vf1 = v1 + J/m1 * n;
	FVector3 vf2 = v2 - J/m2 * n;

	FVector3 wf1 = w1 + I1inv*(cross(r1, J*n));
	FVector3 wf2 = w2 + I2inv*(cross(r2, J*n));

	obj1->physics_body.set_velocity(vf1);
	obj2->physics_body.set_velocity(vf2);

	obj1->physics_body.set_angular_velocity(wf1);
	obj2->physics_body.set_angular_velocity(wf2);
}

// https://research.ncl.ac.uk/game/mastersdegree/gametechnologies/previousinformation/physics6collisionresponse/2017%20Tutorial%206%20-%20Collision%20Response.pdf