#include "PhysicsScene.h"

void PhysicsScene::tick() {
	for (int i = 0; i < objects.size(); i++) {
		for (int j = i+1; j < objects.size(); j++) {
			PhysicalComponent* obj1 = objects[i];
			PhysicalComponent* obj2 = objects[j];

			obj1->physics_body.tick();
			obj2->physics_body.tick();
			obj1->translate(obj1->physics_body.velocity);
			obj2->translate(obj2->physics_body.velocity);

			handle_collision(obj1, obj2);
		}
	}
}

void PhysicsScene::clean_up() {

}

void PhysicsScene::handle_collision(PhysicalComponent* obj1, PhysicalComponent* obj2) {
	if (obj1->get_type() == Component::Type::BoxComponent && obj2->get_type() == Component::Type::BoxComponent) {
		box_box_overlap(
			static_cast<BoxComponent*>(obj1)->box,
			static_cast<BoxComponent*>(obj2)->box
		);
	} else {

		/*bool bounding_box_intersecting = false;
		for (const SimpleTriangle &object1_tri : bounding_box1_tris) {
			for (const SimpleTriangle &object2_tri : bounding_box2_tris) {
				bool intersects = tri_tri_overlap_test_3d(object1_tri, object2_tri);

				if (!bounding_box_intersecting && intersects)
					bounding_box_intersecting = intersects;
			}
		}
		bounding_box_intersecting = true;
		if (bounding_box_intersecting) {
			
		}*/
	}
}

bool PhysicsScene::box_box_overlap(const SimpleBox &box1, const SimpleBox &box2) noexcept {
	//\ X AXIS CHECK
	auto x_extremes = std::minmax_element(box1.vertices.begin(), box1.vertices.end(),
		[](const SimpleVertex &lhs, const SimpleVertex &rhs) {
		return lhs.position.x < rhs.position.x;
	});
	bool x_is_colliding = false;
	

	// Y AXIS CHECK
	auto y_extremes = std::minmax_element(box1.vertices.begin(), box1.vertices.end(),
		[](const SimpleVertex &lhs, const SimpleVertex &rhs) {
		return lhs.position.y < rhs.position.y;
	});
	bool y_is_colliding = false;

	// Z AXIS CHECK
	auto z_extremes = std::minmax_element(box1.vertices.begin(), box1.vertices.end(),
		[](const SimpleVertex &lhs, const SimpleVertex &rhs) {
		return lhs.position.z < rhs.position.z;
	});
	bool z_is_colliding = false;

	return (x_is_colliding || y_is_colliding || z_is_colliding);
}

// https://stackoverflow.com/questions/1496215/triangle-triangle-intersection-in-3d-space

bool PhysicsScene::tri_tri_overlap(const SimpleTriangle &tri1, const SimpleTriangle &tri2) noexcept {
	float dp1, dq1, dr1, dp2, dq2, dr2;
	float v1[3], v2[3];
	float N1[3], N2[3];

	float p1[3] = { tri1.first.position.x,
		tri1.first.position.y,
		tri1.first.position.z };

	float q1[3] = { tri1.second.position.x,
		tri1.second.position.y,
		tri1.second.position.z };

	float r1[3] = { tri1.third.position.x,
		tri1.third.position.y,
		tri1.third.position.z };


	float p2[3] = { tri2.first.position.x,
		tri2.first.position.y,
		tri2.first.position.z };

	float q2[3] = { tri2.second.position.x,
		tri2.second.position.y,
		tri2.second.position.z };

	float r2[3] = { tri2.third.position.x,
		tri2.third.position.y,
		tri2.third.position.z };

	/* Compute distance signs  of p1, q1 and r1 to the plane of
	triangle(p2,q2,r2) */


	SUB(v1,p2,r2)
	SUB(v2,q2,r2)
	CROSS(N2,v1,v2)

	SUB(v1,p1,r2)
	dp1 = DOT(v1,N2);
	SUB(v1,q1,r2)
	dq1 = DOT(v1,N2);
	SUB(v1,r1,r2)
	dr1 = DOT(v1,N2);

	if (((dp1 * dq1) > 0.0f) && ((dp1 * dr1) > 0.0f)) return 0; 

	/* Compute distance signs  of p2, q2 and r2 to the plane of
	triangle(p1,q1,r1) */


	SUB(v1,q1,p1)
	SUB(v2,r1,p1)
	CROSS(N1,v1,v2)

	SUB(v1,p2,r1)
	dp2 = DOT(v1,N1);
	SUB(v1,q2,r1)
	dq2 = DOT(v1,N1);
	SUB(v1,r2,r1)
	dr2 = DOT(v1,N1);

	if (((dp2 * dq2) > 0.0f) && ((dp2 * dr2) > 0.0f)) return 0;

	/* Permutation in a canonical form of T1's vertices */


	if (dp1 > 0.0f) {
		if (dq1 > 0.0f) {
			TRI_TRI_3D(r1,p1,q1,p2,r2,q2,dp2,dr2,dq2)
		} else if (dr1 > 0.0f) {
			TRI_TRI_3D(q1,r1,p1,p2,r2,q2,dp2,dr2,dq2)  
		} else {
			TRI_TRI_3D(p1,q1,r1,p2,q2,r2,dp2,dq2,dr2)
		}
	} else if (dp1 < 0.0f) {
		if (dq1 < 0.0f) {
			TRI_TRI_3D(r1,p1,q1,p2,q2,r2,dp2,dq2,dr2)
		} else if (dr1 < 0.0f) {
			TRI_TRI_3D(q1,r1,p1,p2,q2,r2,dp2,dq2,dr2)
		} else {
			TRI_TRI_3D(p1,q1,r1,p2,r2,q2,dp2,dr2,dq2)
		}
	} else {
		if (dq1 < 0.0f) {
			if (dr1 >= 0.0f) {
				TRI_TRI_3D(q1,r1,p1,p2,r2,q2,dp2,dr2,dq2)
			} else {
				TRI_TRI_3D(p1,q1,r1,p2,q2,r2,dp2,dq2,dr2)
			}
		}
		else if (dq1 > 0.0f) {
			if (dr1 > 0.0f) {
				TRI_TRI_3D(p1,q1,r1,p2,r2,q2,dp2,dr2,dq2)
			} else {
				TRI_TRI_3D(q1,r1,p1,p2,q2,r2,dp2,dq2,dr2)
			}
		} else {
			if (dr1 > 0.0f) {
				TRI_TRI_3D(r1,p1,q1,p2,q2,r2,dp2,dq2,dr2)
			} else if (dr1 < 0.0f) {
				TRI_TRI_3D(r1,p1,q1,p2,r2,q2,dp2,dr2,dq2)
			} else {
				return coplanar_tri_tri3d(p1,q1,r1,p2,q2,r2,N1);
			}
		}
	}
}

bool PhysicsScene::coplanar_tri_tri3d(float p1[3], float q1[3], float r1[3],
	float p2[3], float q2[3], float r2[3],
	float normal_1[3]) noexcept {

	float P1[2],Q1[2],R1[2];
	float P2[2],Q2[2],R2[2];

	float n_x, n_y, n_z;

	n_x = ((normal_1[0]<0)?-normal_1[0]:normal_1[0]);
	n_y = ((normal_1[1]<0)?-normal_1[1]:normal_1[1]);
	n_z = ((normal_1[2]<0)?-normal_1[2]:normal_1[2]);


	/* Projection of the triangles in 3D onto 2D such that the area of
	the projection is maximized. */


	if (( n_x > n_z ) && ( n_x >= n_y )) {
		// Project onto plane YZ

		P1[0] = q1[2]; P1[1] = q1[1];
		Q1[0] = p1[2]; Q1[1] = p1[1];
		R1[0] = r1[2]; R1[1] = r1[1]; 

		P2[0] = q2[2]; P2[1] = q2[1];
		Q2[0] = p2[2]; Q2[1] = p2[1];
		R2[0] = r2[2]; R2[1] = r2[1]; 

	} else if (( n_y > n_z ) && ( n_y >= n_x )) {
		// Project onto plane XZ

		P1[0] = q1[0]; P1[1] = q1[2];
		Q1[0] = p1[0]; Q1[1] = p1[2];
		R1[0] = r1[0]; R1[1] = r1[2]; 

		P2[0] = q2[0]; P2[1] = q2[2];
		Q2[0] = p2[0]; Q2[1] = p2[2];
		R2[0] = r2[0]; R2[1] = r2[2]; 

	} else {
		// Project onto plane XY

		P1[0] = p1[0]; P1[1] = p1[1]; 
		Q1[0] = q1[0]; Q1[1] = q1[1]; 
		R1[0] = r1[0]; R1[1] = r1[1]; 

		P2[0] = p2[0]; P2[1] = p2[1]; 
		Q2[0] = q2[0]; Q2[1] = q2[1]; 
		R2[0] = r2[0]; R2[1] = r2[1]; 
	}

	return tri_tri_overlap_test_2d(P1,Q1,R1,P2,Q2,R2);

}

bool PhysicsScene::tri_tri_overlap_test_2d(float p1[2], float q1[2], float r1[2], 
	float p2[2], float q2[2], float r2[2]) noexcept {
	if (ORIENT_2D(p1,q1,r1) < 0.0f) {
		if ( ORIENT_2D(p2,q2,r2) < 0.0f) {
			return ccw_tri_tri_intersection_2d(p1,r1,q1,p2,r2,q2);
		} else {
			return ccw_tri_tri_intersection_2d(p1,r1,q1,p2,q2,r2);
		}
	} else {
		if (ORIENT_2D(p2,q2,r2) < 0.0f) {
			return ccw_tri_tri_intersection_2d(p1,q1,r1,p2,r2,q2);
		} else {
			return ccw_tri_tri_intersection_2d(p1,q1,r1,p2,q2,r2);
		}
	}
}

bool PhysicsScene::ccw_tri_tri_intersection_2d(float p1[2], float q1[2], float r1[2], 
	float p2[2], float q2[2], float r2[2]) noexcept {
	if (ORIENT_2D(p2,q2,p1) >= 0.0f) {
		if (ORIENT_2D(q2,r2,p1) >= 0.0f) {
			if (ORIENT_2D(r2,p2,p1) >= 0.0f) {
				return 1;
			} else {
				INTERSECTION_TEST_EDGE(p1,q1,r1,p2,q2,r2)
			}
		} else {  
			if (ORIENT_2D(r2,p2,p1) >= 0.0f) {
				INTERSECTION_TEST_EDGE(p1,q1,r1,r2,p2,q2)
			} else {
				INTERSECTION_TEST_VERTEX(p1,q1,r1,p2,q2,r2)
			}
		}
	} else {
		if (ORIENT_2D(q2,r2,p1) >= 0.0f) {
			if (ORIENT_2D(r2,p2,p1) >= 0.0f) {
				INTERSECTION_TEST_EDGE(p1,q1,r1,q2,r2,p2)
			} else {
				INTERSECTION_TEST_VERTEX(p1,q1,r1,q2,r2,p2)
			}
		} else {
			INTERSECTION_TEST_VERTEX(p1,q1,r1,r2,p2,q2)
		}
	}
}

float PhysicsScene::signed_tetrahedron_volume(FVector3 a, FVector3 b, FVector3 c, FVector3 d) noexcept {
	return (1.0f/6.0f)*dot(cross(b-a,c-a),d-a);
}

bool PhysicsScene::triangle_line_collision(const SimpleTriangle &triangle, const Line &line) noexcept {
	FVector3 q1, q2, p1, p2, p3;

	p1 = triangle.first.position;
	p2 = triangle.second.position;
	p3 = triangle.third.position;

	q1 = line.p1;
	q2 = line.p2;

	float a = signed_tetrahedron_volume(q1,p1,p2,p3);
	float b = signed_tetrahedron_volume(q2,p1,p2,p3);
	float c = signed_tetrahedron_volume(q1,q2,p1,p2);
	float d = signed_tetrahedron_volume(q1,q2,p2,p3);
	float e = signed_tetrahedron_volume(q1,q2,p3,p1);


	// If a and b have different signs AND c, d and e have the same sign,
	// then there is an intersection.
	return ((a != b) && (c == d && d == e));
}
