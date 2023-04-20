#pragma once

#include <string>
#include <vector>
#include <Windows.h>
#include <fstream>
#include "Material.h"
#include "globalutil.h"
#include "Component.h"

class MeshData {
public:
	MeshData(const std::vector<Vertex> &vertices, const std::vector<UINT> &indices)
		: vertices(vertices), indices(indices),
		bounding_box(SimpleBox(verts_to_simple_verts(vertices))),
		triangles(split_into_triangles(vertices)) { }

	MeshData(const std::vector<Vertex> &vertices)
		: vertices(vertices), indices(std::vector<UINT>()),
		bounding_box(SimpleBox(verts_to_simple_verts(vertices))),
		triangles(split_into_triangles(vertices)) { }

	bool operator==(const MeshData &mesh) const noexcept {
		return (vertices == mesh.vertices &&
			indices == mesh.indices);
	}

	void operator=(const MeshData &mesh) noexcept {
		vertices = mesh.vertices;
		indices = mesh.indices;
		bounding_box = mesh.bounding_box;
		triangles = mesh.triangles;
	}

	GET SimpleBox get_bounding_box() const noexcept {
		return bounding_box;
	}

	GET const std::vector<Vertex> & get_vertices() const noexcept {
		return vertices;
	}

	GET const std::vector<Triangle> & get_triangles() const noexcept {
		return triangles;
	}

	GET const std::vector<SimpleVertex> & get_physics_vertices() const noexcept {
		return physics_vertices;
	}

	GET const std::vector<SimpleTriangle> & get_physics_triangles() const noexcept {
		return physics_triangles;
	}

	void set_vertices(const std::vector<Vertex> &new_verts, size_t physics_verts_chunk_divisor = 3) {
		if (vertices != new_verts) {
			vertices = new_verts;

			// CALCULATE PHYSCIS VERTICES FROM NEW VERTS.
			// Iterates through new verts with jump length
			// of physics_verts_chunk_divisor and averages the positions
			// to create 1 vertex. There is then physics 1 vertex for every
			// physics_verts_chunk_divisor new_vert.

			//std::vector<SimpleVertex> new_physics_verts;
			//for (size_t i = 0; i < new_verts.size(); i += physics_verts_chunk_divisor) {
			//	std::vector<SimpleVertex> chunk(physics_verts_chunk_divisor);

			//	// Keeping the chunk vector in cause it might be useful later.
			//	for (size_t j = 0; j < physics_verts_chunk_divisor; j++) {
			//		chunk.push_back((SimpleVertex)new_verts[i+j]);
			//	}

			//	FVector3 sum;
			//	for (const SimpleVertex &vert : chunk) {
			//		sum += vert.position;
			//	}
			//	new_physics_verts.push_back(SimpleVertex(sum / chunk.size()));
			//}

			//size_t leftover_verts = new_verts.size() % physics_verts_chunk_divisor - 1;
			//if (leftover_verts > 3) {
			//	/*size_t leftover_divisor = leftover_verts % 3;
			//	size_t leftover_start_index = new_verts.size()-(physics_verts_chunk_divisor-2);

			//	for (size_t i = 0; i < leftover_divisor; i++) {
			//		FVector3 sum;
			//		for (size_t j = leftover_start_index;
			//			j < leftover_start_index + i*3; j++) {
			//			sum += new_verts[j].position;
			//		}
			//		new_physics_verts.push_back(SimpleVertex(sum / leftover_verts));
			//	}*/
			//} else {
			//	for (int i = 0; i < leftover_verts; i++) {
			//		new_physics_verts.pop_back();
			//	}
			//}

			//physics_vertices = new_physics_verts;
			physics_vertices = verts_to_simple_verts(vertices);
			physics_triangles = split_into_simple_triangles(physics_vertices);
			bounding_box = SimpleBox(physics_vertices);

			/*vertices.clear();
			for (const SimpleVertex &vert : bounding_box.vertices) {
				vertices.push_back(Vertex(vert.position, { 0, 0 }, { 0, 0, 0 }));
			}*/
		}
	}

	std::vector<UINT> indices;

private:
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
	std::vector<SimpleVertex> physics_vertices;
	std::vector<SimpleTriangle> physics_triangles;


	SimpleBox bounding_box;
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
	MeshComponent(const FVector3 &position = FVector3(),
		const FVector3 &rotation = FVector3());

	MeshComponent(const Material &material);

	ReadObjFileDataOutput read_obj_file(const ReadObjFileDataInput &read);

	void clean_up() override;

	void compile() noexcept override;

	bool operator==(const MeshComponent &mesh) const noexcept;
	void operator=(const MeshComponent &component) noexcept;

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
