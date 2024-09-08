#pragma once

#include <string>
#include <DirectXMath.h>
#include "../Utility/ObjectStructs.h"
#include "../Utility/SimpleShapes.h"

enum class Unit {
	Meters,						// Distance
	Kilograms,					// Mass
	Newtons,					// Force
	MetersPerSecond,			// Velocity
	MetersPerSecondSquared,		// Acceleration
	Joules						// Kinetic Energy
};

class Force : public FVector3 {
public:
	enum class Type {
		Impulse,
		Constant
	};

	std::string name = "";
	Type type;

	Force(FVector3 vector, std::string name = "",
		Type type = Type::Impulse) : FVector3(vector), name(name), type(type) { }

	bool operator==(const Force &force) const noexcept {
		return (name == force.name && type == force.type &&
			x == force.x && y == force.y && z == force.z);
	}
};

static FVector3 cross(const FVector3 &a, const FVector3 &b) {
	return FVector3{
		a.y * b.z - a.z * b.y,
		-(a.x * b.z - a.z * b.x),
		a.x * b.y - a.y * b.x
	};
}

static float dot(const FVector3 &v1, const FVector3 &v2){
	return (v1.x*v2.x) + (v1.y*v2.y) + (v1.z*v2.z);
}

static FVector3 dot(const XMMATRIX &mat, const FVector3 &v) {
	return XMVector3TransformCoord(v.to_xmvec(), mat);
}

//static float normalize(float f) {
//	return 1.0f / f;
//}

static float distance(const FVector3 &p1, const FVector3 &p2) noexcept {
	return (p2 - p1).length();
}

static FVector3 operator*(const XMMATRIX &mat, const FVector3 &vec) {
	return dot(mat, vec);
}

static FVector3 transform_vector(FVector3 vector, Transform transform) noexcept {
	XMVECTOR vec = XMVector3Transform(vector.to_xmvec(),
		XMMatrixTranspose(transform));

	FVector3 ret;

	ret.x = XMVectorGetX(vec);
	ret.y = XMVectorGetY(vec);
	ret.z = XMVectorGetZ(vec);

	return ret;
}

//static std::vector<SimpleVertex> tris_to_simple_verts(const std::vector<Triangle> &tris) noexcept {
//	std::vector<SimpleVertex> ret;
//	for (const Triangle &tri : tris) {
//		ret.push_back((SimpleVertex)tri.first);
//		ret.push_back((SimpleVertex)tri.second);
//		ret.push_back((SimpleVertex)tri.third);
//	}
//
//	return ret;
//}

static std::vector<SimpleVertex> simple_tris_to_simple_verts(const std::vector<SimpleTriangle> &tris) noexcept {
	std::vector<SimpleVertex> ret;
	for (const SimpleTriangle &tri : tris) {
		ret.push_back(tri.first.position);
		ret.push_back(tri.second.position);
		ret.push_back(tri.third.position);
	}

	return ret;
}

//static std::vector<SimpleTriangle> tris_to_simple_tris(const std::vector<Triangle> &tris) {
//	std::vector<SimpleTriangle> ret;
//	for (const Triangle &tri : tris) {
//		ret.push_back(tri);
//	}
//
//	return ret;
//}
//
//static std::vector<SimpleVertex> verts_to_simple_verts(const std::vector<Vertex> &verts) {
//	std::vector<SimpleVertex> ret;
//	for (const Vertex &vert : verts) {
//		ret.push_back(vert);
//	}
//
//	return ret;
//}

static std::vector<Vertex> transform_vertices(const std::vector<Vertex> &verts, const Transform &transform) {
	std::vector<Vertex> ret = verts;

	for (Vertex &vertex : ret) {
		vertex.position = transform_vector(vertex.position, transform);
	}

	return ret;
}

static std::vector<SimpleVertex> transform_simple_vertices(const std::vector<SimpleVertex> &verts, const Transform &transform) {
	std::vector<SimpleVertex> ret = verts;

	for (SimpleVertex &vertex : ret) {
		vertex.position = transform_vector(vertex.position, transform);
	}

	return ret;
}

static std::vector<Triangle> transform_tris(const std::vector<Triangle> &tris, const Transform &transform) {
	std::vector<Triangle> ret = tris;

	for (Triangle &tri : ret) {
		tri.first.position = transform_vector(tri.first.position, transform);
		tri.second.position = transform_vector(tri.second.position, transform);
		tri.third.position = transform_vector(tri.third.position, transform);
	}

	return ret;
}

static std::vector<SimpleTriangle> transform_simple_tris(const std::vector<SimpleTriangle> &tris, const Transform &transform) {
	std::vector<SimpleTriangle> ret = tris;

	for (SimpleTriangle &tri : ret) {
		tri.first.position = transform_vector(tri.first.position, transform);
		tri.second.position = transform_vector(tri.second.position, transform);
		tri.third.position = transform_vector(tri.third.position, transform);
	}

	return ret;
}

static SimpleBox transform_simple_box(SimpleBox box, const Transform &transform) {
	auto v = transform_simple_vertices(std::vector<SimpleVertex>{box.vertices.begin(), box.vertices.end()}, transform);
	std::copy_n(std::make_move_iterator(v.begin()), box.vertices.size(), box.vertices.begin());
	return box;
}
