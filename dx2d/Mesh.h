#pragma once

#include "globalutil.h"

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

class Mesh {
public:
	Mesh() { }
	Mesh(const std::vector<Vertex> &vertices, const std::vector<UINT> &indices);
	Mesh(const std::vector<Vertex> &vertices);

	void compile();

	bool operator==(const Mesh &mesh) const noexcept;
	void operator=(const Mesh &mesh) noexcept;

	GET SimpleBox get_bounding_box() const noexcept;
	GET const std::vector<Vertex> & get_vertices() const noexcept;
	GET const std::vector<Triangle> & get_triangles() const noexcept;
	GET const std::vector<SimpleVertex> & get_physics_vertices() const noexcept;
	GET const std::vector<SimpleTriangle> & get_physics_triangles() const noexcept;

	void set_vertices(const std::vector<Vertex> &new_verts, size_t physics_verts_chunk_divisor = 3);

	//ReadObjFileDataOutput read_obj_file(const ReadObjFileDataInput &read);

	std::vector<UINT> indices;

private:
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
	std::vector<SimpleVertex> physics_vertices;
	std::vector<SimpleTriangle> physics_triangles;


	SimpleBox bounding_box;
};