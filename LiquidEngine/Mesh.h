#pragma once

#include "globalutil.h"

class Mesh {
public:
	Mesh() { }
	Mesh(const std::vector<Vertex> &vertices, const std::vector<UINT> &indices);
	Mesh(const std::vector<Vertex> &vertices);
	Mesh(const std::string &file);

	void compile();

	bool operator==(const Mesh &mesh) const noexcept;
	void operator=(const Mesh &mesh) noexcept;

	GET SimpleBox get_bounding_box() const noexcept;
	GET const std::vector<Vertex> & get_vertices() const noexcept;
	GET const std::vector<Triangle> & get_triangles() const noexcept;
	GET const std::vector<SimpleVertex> & get_physics_vertices() const noexcept;
	GET const std::vector<SimpleTriangle> & get_physics_triangles() const noexcept;

	void set_vertices(const std::vector<Vertex> &new_verts, size_t physics_verts_chunk_divisor = 3);
	void set_vertices(const std::string &file, size_t physics_verts_chunk_divisor = 3);

	std::vector<UINT> indices;

private:
	std::vector<Vertex> vertices;
	std::vector<Triangle> triangles;
	std::vector<SimpleVertex> physics_vertices;
	std::vector<SimpleTriangle> physics_triangles;


	SimpleBox bounding_box;
};