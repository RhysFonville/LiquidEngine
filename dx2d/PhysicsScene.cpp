#include "PhysicsScene.h"

PhysicsScene::PhysicsScene(ObjectVector &objects) : objects(objects) {}

void PhysicsScene::tick() noexcept {
	for (std::shared_ptr<Object> &object1 : *objects) {
		handle_mechanics(*object1);
		for (std::shared_ptr<Object> &object2 : *objects) {
			if (*object1 != *object2) {
				handle_mechanics(*object2);
				handle_collision(*object1, *object2);
			}
		}
	}
}

void PhysicsScene::clean_up() {

}

void PhysicsScene::handle_mechanics(Object &object) {
	object.mechanics.acceleration = object.mechanics.get_net_force() / object.mechanics.get_mass();
	object.mechanics.velocity += object.mechanics.acceleration;

	FVector3 old_position = object.get_transform().position;

	object.set_position(old_position + object.mechanics.velocity);

	object.mechanics.momentum = object.mechanics.get_mass() * object.mechanics.velocity;
	object.mechanics.speed = distance(object.mechanics.previous_position, object.get_transform().position);
	object.mechanics.kinetic_energy = object.mechanics.get_mass()/2 * powf(object.mechanics.speed, 2);
}

void PhysicsScene::handle_collision(Object &object1, Object &object2) {
	if (object1.has_component<MeshComponent>() && object2.has_component<MeshComponent>()) {
		MeshComponent mesh_component1 = *object1.get_component<MeshComponent>();
		MeshComponent mesh_component2 = *object2.get_component<MeshComponent>();
		
		std::vector<SimpleTriangle> bounding_box1_tris = mesh_component1.mesh_data.get_bounding_box().split_into_triangles();
		std::vector<SimpleTriangle> bounding_box2_tris = mesh_component1.mesh_data.get_bounding_box().split_into_triangles();
		bounding_box1_tris = transform_simple_tris(bounding_box1_tris, object1.get_transform());
		bounding_box1_tris = transform_simple_tris(bounding_box2_tris, object2.get_transform());

		bool bounding_box_intersecting = false;
		for (const SimpleTriangle &object1_tri : bounding_box1_tris) {
			for (const SimpleTriangle &object2_tri : bounding_box2_tris) {
				bool intersects = (
					tri_tri_overlap_test_3d(object1_tri, object2_tri)
				);

				if (!bounding_box_intersecting && intersects)
					bounding_box_intersecting = intersects;
			}
		}
		bounding_box_intersecting = true;
		if (bounding_box_intersecting) {
			std::vector<SimpleTriangle> object1_tris =
				split_into_simple_triangles(
					transform_simple_vertices(
						mesh_component1.mesh_data.get_physics_vertices(),
						object1.get_transform()
					)
				);
			std::vector<SimpleTriangle> object2_tris =
				split_into_simple_triangles(
					transform_simple_vertices(
						mesh_component2.mesh_data.get_physics_vertices(),
						object1.get_transform()
					)
				);

			bool intersecting = false;
			for (const SimpleTriangle &object1_tri : object1_tris) {
				for (const SimpleTriangle &object2_tri : object2_tris) {
					bool intersects = (
						tri_tri_overlap_test_3d(object1_tri, object2_tri)
					);

					if (intersects) {
						intersecting = true;
						break;
					}
				}
				if (intersecting)
					break;
			}

			/*auto approx_closest_tris = approximate_closest_tris(
				mesh_component1.mesh_data, mesh_component2.mesh_data,
				object1.get_transform(), object2.get_transform()
			);
			std::vector<SimpleTriangle> object1_closest_tris = approx_closest_tris.first;
			std::vector<SimpleTriangle> object2_closest_tris = approx_closest_tris.second;

			bool intersecting = false;
			for (const SimpleTriangle &object1_tri : object1_closest_tris) {
				for (const SimpleTriangle &object2_tri : object2_closest_tris) {
					bool intersects = (
						tri_tri_overlap_test_3d(object1_tri, object2_tri)
					);

					if (intersects) {
						intersecting = true;
						break;
					}
				}
				if (intersecting)
					break;
			}*/

			if (intersecting) {
				object1.get_component<MeshComponent>()->material.kd = Color(0, 255, 0);
				object2.get_component<MeshComponent>()->material.kd = Color(0, 255, 0);

				object1.get_component<MeshComponent>()->material.ks = Color(0, 255, 0);
				object2.get_component<MeshComponent>()->material.ks = Color(0, 255, 0);
			} else {
				object1.get_component<MeshComponent>()->material.kd = Color(255, 0, 0);
				object2.get_component<MeshComponent>()->material.kd = Color(255, 0, 0);

				object1.get_component<MeshComponent>()->material.ks = Color(255, 0, 0);
				object2.get_component<MeshComponent>()->material.ks = Color(255, 0, 0);
			}
		}
	}
}

// https://stackoverflow.com/questions/1496215/triangle-triangle-intersection-in-3d-space

bool PhysicsScene::tri_tri_overlap_test_3d(const SimpleTriangle &tri1, const SimpleTriangle &tri2) noexcept {
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

std::pair<std::vector<SimpleTriangle>, std::vector<SimpleTriangle>> PhysicsScene::approximate_closest_tris(const MeshData &mesh1, const MeshData &mesh2, const Transform &obj1_transform, const Transform &obj2_transform) {
	std::pair<std::vector<SimpleTriangle>, std::vector<SimpleTriangle>> ret;
	
	SimpleBox box1 = mesh1.get_bounding_box();
	SimpleBox box2 = mesh2.get_bounding_box();

	std::pair<SimpleVertex, SimpleVertex> closest_box_verts = std::make_pair(
		box1.vertices[0], box2.vertices[0]
	);
	
	float closest_box_verts_dist = distance(
		transform_vector(closest_box_verts.first.position, obj1_transform),
		transform_vector(closest_box_verts.second.position, obj2_transform)
	);
	for (SimpleVertex box1_vert : mesh1.get_bounding_box().vertices) {
		for (SimpleVertex box2_vert : mesh2.get_bounding_box().vertices) {
			if (float dist = distance(transform_vector(box1_vert.position, obj1_transform),
							transform_vector(box2_vert.position, obj2_transform));
				dist < closest_box_verts_dist) {

				closest_box_verts = std::make_pair(box1_vert, box2_vert);
				closest_box_verts_dist = dist;
			}
		}
	}

	for (SimpleTriangle obj1_tri : mesh1.get_physics_triangles()) {
		if (obj1_tri.contains(closest_box_verts.first)) {
			ret.first.push_back(obj1_tri);
		}
	}
	for (SimpleTriangle obj2_tri : mesh1.get_physics_triangles()) {
		if (obj2_tri.contains(closest_box_verts.first)) {
			ret.first.push_back(obj2_tri);
		}
	}

	return ret;
}
