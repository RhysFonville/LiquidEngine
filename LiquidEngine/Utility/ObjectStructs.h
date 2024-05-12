#pragma once

#include <vector>
#include "Vectors.h"

struct Transform {
	FVector3 position = { 0, 0, 0 };
	FVector3 rotation = { 0, 0, 0 };
	FVector3 size = { 1, 1, 1 };

	Transform(FVector3 position)
		: position(position), rotation({ 0.0f, 0.0f, 0.0f }), size({ 1.0f, 1.0f, 1.0f }) { }
	Transform()
		: position({ 0.0f, 0.0f, 0.0f }), rotation({ 0.0f, 0.0f, 0.0f }), size({ 1.0f, 1.0f, 1.0f }) { }
	Transform(FVector3 position, FVector3 rotation)
		: position(position), rotation(rotation), size({ 1.0f, 1.0f, 1.0f }) { }
	Transform(FVector3 position, FVector3 rotation, FVector3 size)
		: position(position), rotation(rotation), size(size) { }

	operator XMMATRIX() const noexcept {
		return DirectX::XMMatrixTranspose(
			DirectX::XMMatrixRotationRollPitchYaw(rotation.y, rotation.x, rotation.z)  *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z) *
			DirectX::XMMatrixScaling(size.x, size.y, size.z)
		);
	}

	bool operator==(const Transform &transform) const noexcept {
		return (position == transform.position &&
			rotation == transform.rotation &&
			size == transform.size);
	}
};

struct Vertex {
	FVector3 position = FVector3();
	FVector2 texcoord = FVector2();
	FVector3 normal = FVector3();
	FVector3 tangent = FVector3();
	//FVector3 bitangent = FVector3();

	Vertex() { }

	Vertex(float x, float y, float z) : position(x, y, z) { }

	Vertex(float x, float y, float z, float u, float v)
		: position(x, y, z), texcoord(u, v) { }

	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: position(x, y, z), texcoord(u, v), normal(nx, ny, nz) { }

	Vertex(FVector3 position, FVector2 texcoord, FVector3 normal)
		: position(position), texcoord(texcoord), normal(normal) { }

	bool operator==(const Vertex &vertex) const noexcept {
		return (position == vertex.position &&
			texcoord == vertex.texcoord &&
			normal == vertex.normal &&
			tangent == vertex.tangent);
	}
};

struct Triangle {
	Vertex first;
	Vertex second;
	Vertex third;
};

static std::vector<Vertex> tris_to_verts(const std::vector<Triangle> &tris) {
	std::vector<Vertex> ret;
	for (const Triangle &tri : tris) {
		ret.push_back(tri.first);
		ret.push_back(tri.second);
		ret.push_back(tri.third);
	}

	return ret;
}

static std::vector<Triangle> split_into_triangles(const std::vector<Vertex> &verts) noexcept {
	std::vector<Triangle> ret;

	for (size_t i = 0; i < verts.size(); i += 3){
		if (i + 2 < verts.size()) {
			ret.push_back({
				verts[i+0],
				verts[i+1],
				verts[i+2]
			});
		}
	}

	return ret;
}
