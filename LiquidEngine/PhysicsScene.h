#pragma once

#include "Components/BoxComponent.h"

#define CROSS(dest,v1,v2)                       \
			   dest[0]=v1[1]*v2[2]-v1[2]*v2[1]; \
			   dest[1]=v1[2]*v2[0]-v1[0]*v2[2]; \
			   dest[2]=v1[0]*v2[1]-v1[1]*v2[0];

#define DOT(v1,v2) (v1[0]*v2[0]+v1[1]*v2[1]+v1[2]*v2[2])



#define SUB(dest,v1,v2) dest[0]=v1[0]-v2[0]; \
						dest[1]=v1[1]-v2[1]; \
						dest[2]=v1[2]-v2[2]; 


#define SCALAR(dest,alpha,v) dest[0] = alpha * v[0]; \
							 dest[1] = alpha * v[1]; \
							 dest[2] = alpha * v[2];



#define CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2) {\
  SUB(v1,p2,q1)\
  SUB(v2,p1,q1)\
  CROSS(N1,v1,v2)\
  SUB(v1,q2,q1)\
  if (DOT(v1,N1) > 0.0f) return 0;\
  SUB(v1,p2,p1)\
  SUB(v2,r1,p1)\
  CROSS(N1,v1,v2)\
  SUB(v1,r2,p1) \
  if (DOT(v1,N1) > 0.0f) return 0;\
  else return 1; }



// Permutation in a canonical form of T2's vertices

#define TRI_TRI_3D(p1,q1,r1,p2,q2,r2,dp2,dq2,dr2) { \
  if (dp2 > 0.0f) { \
	 if (dq2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,r2,p2,q2) \
	 else if (dr2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,q2,r2,p2)\
	 else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2) }\
  else if (dp2 < 0.0f) { \
	if (dq2 < 0.0f) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2)\
	else if (dr2 < 0.0f) CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2)\
	else CHECK_MIN_MAX(p1,r1,q1,p2,q2,r2)\
  } else { \
	if (dq2 < 0.0f) { \
	  if (dr2 >= 0.0f)  CHECK_MIN_MAX(p1,r1,q1,q2,r2,p2)\
	  else CHECK_MIN_MAX(p1,q1,r1,p2,q2,r2)\
	} \
	else if (dq2 > 0.0f) { \
	  if (dr2 > 0.0f) CHECK_MIN_MAX(p1,r1,q1,p2,q2,r2)\
	  else  CHECK_MIN_MAX(p1,q1,r1,q2,r2,p2)\
	} \
	else  { \
	  if (dr2 > 0.0f) CHECK_MIN_MAX(p1,q1,r1,r2,p2,q2)\
	  else if (dr2 < 0.0f) CHECK_MIN_MAX(p1,r1,q1,r2,p2,q2)\
	  else return coplanar_tri_tri3d(p1,q1,r1,p2,q2,r2,N1);\
	 }}}

#define CONSTRUCT_INTERSECTION(p1,q1,r1,p2,q2,r2) { \
  SUB(v1,q1,p1) \
  SUB(v2,r2,p1) \
  CROSS(N,v1,v2) \
  SUB(v,p2,p1) \
  if (DOT(v,N) > 0.0f) {\
	SUB(v1,r1,p1) \
	CROSS(N,v1,v2) \
	if (DOT(v,N) <= 0.0f) { \
	  SUB(v2,q2,p1) \
	  CROSS(N,v1,v2) \
	  if (DOT(v,N) > 0.0f) { \
  SUB(v1,p1,p2) \
  SUB(v2,p1,r1) \
  alpha = DOT(v1,N2) / DOT(v2,N2); \
  SCALAR(v1,alpha,v2) \
  SUB(source,p1,v1) \
  SUB(v1,p2,p1) \
  SUB(v2,p2,r2) \
  alpha = DOT(v1,N1) / DOT(v2,N1); \
  SCALAR(v1,alpha,v2) \
  SUB(target,p2,v1) \
  return 1; \
	  } else { \
  SUB(v1,p2,p1) \
  SUB(v2,p2,q2) \
  alpha = DOT(v1,N1) / DOT(v2,N1); \
  SCALAR(v1,alpha,v2) \
  SUB(source,p2,v1) \
  SUB(v1,p2,p1) \
  SUB(v2,p2,r2) \
  alpha = DOT(v1,N1) / DOT(v2,N1); \
  SCALAR(v1,alpha,v2) \
  SUB(target,p2,v1) \
  return 1; \
	  } \
	} else { \
	  return 0; \
	} \
  } else { \
	SUB(v2,q2,p1) \
	CROSS(N,v1,v2) \
	if (DOT(v,N) < 0.0f) { \
	  return 0; \
	} else { \
	  SUB(v1,r1,p1) \
	  CROSS(N,v1,v2) \
	  if (DOT(v,N) >= 0.0f) { \
  SUB(v1,p1,p2) \
  SUB(v2,p1,r1) \
  alpha = DOT(v1,N2) / DOT(v2,N2); \
  SCALAR(v1,alpha,v2) \
  SUB(source,p1,v1) \
  SUB(v1,p1,p2) \
  SUB(v2,p1,q1) \
  alpha = DOT(v1,N2) / DOT(v2,N2); \
  SCALAR(v1,alpha,v2) \
  SUB(target,p1,v1) \
  return 1; \
	  } else { \
  SUB(v1,p2,p1) \
  SUB(v2,p2,q2) \
  alpha = DOT(v1,N1) / DOT(v2,N1); \
  SCALAR(v1,alpha,v2) \
  SUB(source,p2,v1) \
  SUB(v1,p1,p2) \
  SUB(v2,p1,q1) \
  alpha = DOT(v1,N2) / DOT(v2,N2); \
  SCALAR(v1,alpha,v2) \
  SUB(target,p1,v1) \
  return 1; \
	  }}}} 



#define TRI_TRI_INTER_3D(p1,q1,r1,p2,q2,r2,dp2,dq2,dr2) { \
  if (dp2 > 0.0f) { \
	 if (dq2 > 0.0f) CONSTRUCT_INTERSECTION(p1,r1,q1,r2,p2,q2) \
	 else if (dr2 > 0.0f) CONSTRUCT_INTERSECTION(p1,r1,q1,q2,r2,p2)\
	 else CONSTRUCT_INTERSECTION(p1,q1,r1,p2,q2,r2) }\
  else if (dp2 < 0.0f) { \
	if (dq2 < 0.0f) CONSTRUCT_INTERSECTION(p1,q1,r1,r2,p2,q2)\
	else if (dr2 < 0.0f) CONSTRUCT_INTERSECTION(p1,q1,r1,q2,r2,p2)\
	else CONSTRUCT_INTERSECTION(p1,r1,q1,p2,q2,r2)\
  } else { \
	if (dq2 < 0.0f) { \
	  if (dr2 >= 0.0f)  CONSTRUCT_INTERSECTION(p1,r1,q1,q2,r2,p2)\
	  else CONSTRUCT_INTERSECTION(p1,q1,r1,p2,q2,r2)\
	} \
	else if (dq2 > 0.0f) { \
	  if (dr2 > 0.0f) CONSTRUCT_INTERSECTION(p1,r1,q1,p2,q2,r2)\
	  else  CONSTRUCT_INTERSECTION(p1,q1,r1,q2,r2,p2)\
	} \
	else  { \
	  if (dr2 > 0.0f) CONSTRUCT_INTERSECTION(p1,q1,r1,r2,p2,q2)\
	  else if (dr2 < 0.0f) CONSTRUCT_INTERSECTION(p1,r1,q1,r2,p2,q2)\
	  else { \
		*coplanar = 1; \
  return coplanar_tri_tri3d(p1,q1,r1,p2,q2,r2,N1);\
	 } \
  }} }

#define ORIENT_2D(a, b, c)  ((a[0]-c[0])*(b[1]-c[1])-(a[1]-c[1])*(b[0]-c[0]))

#define INTERSECTION_TEST_VERTEX(P1, Q1, R1, P2, Q2, R2) {\
  if (ORIENT_2D(R2,P2,Q1) >= 0.0f)\
	if (ORIENT_2D(R2,Q2,Q1) <= 0.0f)\
	  if (ORIENT_2D(P1,P2,Q1) > 0.0f) {\
  if (ORIENT_2D(P1,Q2,Q1) <= 0.0f) return 1; \
  else return 0; \
	  } else {\
  if (ORIENT_2D(P1,P2,R1) >= 0.0f)\
	if (ORIENT_2D(Q1,R1,P2) >= 0.0f) return 1; \
	else return 0;\
  else return 0; \
	  }\
	else \
	  if (ORIENT_2D(P1,Q2,Q1) <= 0.0f)\
  if (ORIENT_2D(R2,Q2,R1) <= 0.0f)\
	if (ORIENT_2D(Q1,R1,Q2) >= 0.0f) return 1; \
	else return 0;\
  else return 0;\
	  else return 0;\
  else\
	if (ORIENT_2D(R2,P2,R1) >= 0.0f) \
	  if (ORIENT_2D(Q1,R1,R2) >= 0.0f)\
  if (ORIENT_2D(P1,P2,R1) >= 0.0f) return 1;\
  else return 0;\
	  else \
  if (ORIENT_2D(Q1,R1,Q2) >= 0.0f) {\
	if (ORIENT_2D(R2,R1,Q2) >= 0.0f) return 1; \
	else return 0; }\
  else return 0; \
	else  return 0; \
 };



#define INTERSECTION_TEST_EDGE(P1, Q1, R1, P2, Q2, R2) { \
  if (ORIENT_2D(R2,P2,Q1) >= 0.0f) {\
	if (ORIENT_2D(P1,P2,Q1) >= 0.0f) { \
		if (ORIENT_2D(P1,Q1,R2) >= 0.0f) return 1; \
		else return 0; \
	} else { \
	  if (ORIENT_2D(Q1,R1,P2) >= 0.0f){ \
  if (ORIENT_2D(R1,P1,P2) >= 0.0f) return 1; else return 0; \
	  } \
	  else return 0; \
	} \
  } else {\
	if (ORIENT_2D(R2,P2,R1) >= 0.0f) {\
	  if (ORIENT_2D(P1,P2,R1) >= 0.0f) {\
  if (ORIENT_2D(P1,R1,R2) >= 0.0f) return 1;  \
  else {\
	if (ORIENT_2D(Q1,R1,R2) >= 0.0f) return 1; else return 0; \
  } \
	  }\
	  else  return 0; \
	}\
	else return 0; \
  } \
}

class PhysicsScene {
public:
	PhysicsScene() { }

	void tick();

	void clean_up();

	void handle_collision(PhysicalComponent* obj1, PhysicalComponent* obj2);

	static bool box_box_overlap(const SimpleBox &box1, const SimpleBox &box2) noexcept;

	static bool tri_tri_overlap(const SimpleTriangle &tri1, const SimpleTriangle &tri2) noexcept;

	static bool coplanar_tri_tri3d(float p1[3], float q1[3], float r1[3],
		float p2[3], float q2[3], float r2[3],
		float normal_1[3]) noexcept;

	static bool tri_tri_overlap_test_2d(float p1[2], float q1[2], float r1[2], 
		float p2[2], float q2[2], float r2[2]) noexcept;

	static bool ccw_tri_tri_intersection_2d(float p1[2], float q1[2], float r1[2], 
		float p2[2], float q2[2], float r2[2]) noexcept;

	static float signed_tetrahedron_volume(FVector3 a, FVector3 b, FVector3 c, FVector3 d) noexcept;
	static bool triangle_line_collision(const SimpleTriangle &triangle, const Line &line) noexcept;

	std::vector<PhysicalComponent*> objects;
};
