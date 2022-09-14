#pragma once

#include <string>
#include <vector>
#include <Windows.h>
#include <fstream>
#include "Material.h"
#include "globalutil.h"
#include "Component.h"

using namespace Geometry;

static std::vector<Triangle> split_into_triangles(const std::vector<Vertex> &tris) noexcept {
	std::vector<Triangle> ret;

	for (size_t i = 0; i < tris.size(); i += 3){
		if (i + 2 <= tris.size()) {
			ret.push_back({
				tris[i+0],
				tris[i+1],
				tris[i+2]
			});
		}
	}

	return ret;
}

struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<USHORT> indices;

	Box bounding_box;

	bool operator==(const MeshData &mesh) const noexcept {
		return (vertices == mesh.vertices &&
			indices == mesh.indices);
	}

	std::vector<Triangle> split_mesh_into_triangles() const noexcept {
		return split_into_triangles(vertices);
	}

	std::vector<Vertex> transform(const Transform &transform) {
		std::vector<Vertex> ret = vertices;
		
		for (Vertex &vertex : ret) {
			vertex.position = XMVector3Transform(vertex.position,
				XMMatrixTranspose(transform));
		}

		return ret;
	}

	Box get_bounding_box() {
		auto x_extremes = std::minmax_element(vertices.begin(), vertices.end(),
			[](const Vertex &lhs, const Vertex &rhs) {
				return lhs.position.x < rhs.position.x;
			});

		auto y_extremes = std::minmax_element(vertices.begin(), vertices.end(),
			[](const Vertex &lhs, const Vertex &rhs) {
				return lhs.position.y < rhs.position.y;
			});

		auto z_extremes = std::minmax_element(vertices.begin(), vertices.end(),
			[](const Vertex &lhs, const Vertex &rhs) {
				return lhs.position.y < rhs.position.y;
			});

		Vertex v1(	x_extremes.first->position.x,
					y_extremes.first->position.y,
					z_extremes.first->position.z
		);
		Vertex v2(	x_extremes.first->position.x,
					y_extremes.second->position.y,
					z_extremes.first->position.z
		);
		Vertex v3(	x_extremes.second->position.x,
					y_extremes.second->position.y,
					z_extremes.first->position.z
		);
		Vertex v4(	x_extremes.second->position.x,
					y_extremes.first->position.y,
					z_extremes.first->position.z
		);
		Vertex v5(	x_extremes.first->position.x,
					y_extremes.first->position.y,
					z_extremes.second->position.z
		);
		Vertex v6(	x_extremes.first->position.x,
					y_extremes.second->position.y,
					z_extremes.second->position.z
		);
		Vertex v7(	x_extremes.second->position.x,
					y_extremes.second->position.y,
					z_extremes.second->position.z
		);
		Vertex v8(	x_extremes.second->position.x,
					y_extremes.first->position.y,
					z_extremes.second->position.z
		);

		return Box({ v1, v2, v3, v4, v5, v6, v7, v8 });
	}
};

struct ReadObjFileDataOutput {
	size_t vertex_index_offset = 0;
	size_t texcoord_index_offset = 0;
	size_t normal_index_offset = 0;

	ReadObjFileDataOutput() { }
	ReadObjFileDataOutput(size_t vio, size_t tcio, size_t nio)
		: vertex_index_offset(vio), texcoord_index_offset(tcio),
		normal_index_offset(nio) { }

	void operator+=(const ReadObjFileDataOutput &lhs) {
		vertex_index_offset += lhs.vertex_index_offset;
		texcoord_index_offset += lhs.texcoord_index_offset;
		normal_index_offset += lhs.normal_index_offset;
	}
};

struct ReadObjFileDataInput {
	std::vector<std::string> contents;
	ReadObjFileDataOutput offsets;
	std::wstring file_name;

	ReadObjFileDataInput() { }
	ReadObjFileDataInput(std::vector<std::string> contents,
		ReadObjFileDataOutput offsets = ReadObjFileDataOutput(), std::wstring file_name = L"")
		: contents(contents), offsets(offsets), file_name(file_name) { }
};

class MeshComponent : public Component {
public:
	MeshComponent(const FPosition3 &position = FPosition3(),
		const FRotation3 &rotation = FRotation3());

	MeshComponent(const Material &material);

	ReadObjFileDataOutput read_obj_file(const ReadObjFileDataInput &read);

	void clean_up() override;

	void compile() noexcept override;

	bool operator==(const MeshComponent &appearance) const noexcept;

	Material material;

	MeshData mesh_data = {
		 {
			// Front Face
			Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f,-1.0f, -1.0f, -1.0f),
			Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f,-1.0f,  1.0f, -1.0f),
			Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f, 1.0f,  1.0f, -1.0f),
			Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 1.0f, 1.0f, -1.0f, -1.0f),

			// Back Face
			Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f,-1.0f, -1.0f, 1.0f),
			Vertex( 1.0f, -1.0f, 1.0f, 0.0f, 1.0f, 1.0f, -1.0f, 1.0f),
			Vertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f, 1.0f,  1.0f, 1.0f),
			Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f,-1.0f,  1.0f, 1.0f),

			// Top Face
			Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f,-1.0f, 1.0f, -1.0f),
			Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f,-1.0f, 1.0f,  1.0f),
			Vertex( 1.0f, 1.0f,  1.0f, 1.0f, 0.0f, 1.0f, 1.0f,  1.0f),
			Vertex( 1.0f, 1.0f, -1.0f, 1.0f, 1.0f, 1.0f, 1.0f, -1.0f),

			// Bottom Face
			Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,-1.0f, -1.0f, -1.0f),
			Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
			Vertex( 1.0f, -1.0f,  1.0f, 0.0f, 0.0f, 1.0f, -1.0f,  1.0f),
			Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f,-1.0f, -1.0f,  1.0f),

			// Left Face
			Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f,-1.0f, -1.0f,  1.0f),
			Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f,-1.0f,  1.0f,  1.0f),
			Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f,-1.0f,  1.0f, -1.0f),
			Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f,-1.0f, -1.0f, -1.0f),

			// Right Face
			Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 1.0f, 1.0f, -1.0f, -1.0f),
			Vertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f, 1.0f,  1.0f, -1.0f),
			Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f, 1.0f,  1.0f,  1.0f),
			Vertex( 1.0f, -1.0f,  1.0f, 1.0f, 1.0f, 1.0f, -1.0f,  1.0f)
		},
		{
			// Front Face
			0,  1,  2,
			0,  2,  3,

			// Back Face
			4,  5,  6,
			4,  6,  7,

			// Top Face
			8,  9, 10,
			8, 10, 11,

			// Bottom Face
			12, 13, 14,
			12, 14, 15,

			// Left Face
			16, 17, 18,
			16, 18, 19,

			// Right Face
			20, 21, 22,
			20, 22, 23
		}
	};

	static const Type component_type = Type::MeshComponent;
};
