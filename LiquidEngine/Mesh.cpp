#include "Mesh.h"

Mesh::Mesh(const std::vector<Vertex> &vertices, const std::vector<UINT> &indices)
	: vertices(vertices), indices(indices),
	bounding_box(SimpleBox(verts_to_simple_verts(vertices))),
	triangles(split_into_triangles(vertices)) { compile(); }

Mesh::Mesh(const std::vector<Vertex> &vertices)
	: vertices(vertices), indices(std::vector<UINT>()),
	bounding_box(SimpleBox(verts_to_simple_verts(vertices))),
	triangles(split_into_triangles(vertices)) { compile(); }

void Mesh::compile() {
	for (size_t i = 0; i < vertices.size(); i += 3) {
		if (i <= vertices.size()) {
			// Shortcuts for vertices
			FVector3 v0 = vertices[i+0].position;
			FVector3 v1 = vertices[i+1].position;
			FVector3 v2 = vertices[i+2].position;

			// Shortcuts for UVs
			FVector2 uv0 = vertices[i+0].texcoord;
			FVector2 uv1 = vertices[i+1].texcoord;
			FVector2 uv2 = vertices[i+2].texcoord;

			// Edges of the triangle : position delta
			FVector3 deltaPos1 = v1-v0;
			FVector3 deltaPos2 = v2-v0;

			// UV delta
			FVector2 deltaUV1 = uv1-uv0;
			FVector2 deltaUV2 = uv2-uv0;

			float r =     1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			FVector3 tangent =   (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			//FVector3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

			vertices[i+0].tangent = tangent;
			vertices[i+1].tangent = tangent;
			vertices[i+2].tangent = tangent;

			/*verts[i+0].bitangent = bitangent;
			verts[i+1].bitangent = bitangent;
			verts[i+2].bitangent = bitangent;*/
		}
	}

	indices = std::vector<UINT>();
}

bool Mesh::operator==(const Mesh &mesh) const noexcept {
	return (vertices == mesh.vertices &&
		indices == mesh.indices);
}

void Mesh::operator=(const Mesh &mesh) noexcept {
	vertices = mesh.vertices;
	indices = mesh.indices;
	bounding_box = mesh.bounding_box;
	triangles = mesh.triangles;
}

GET SimpleBox Mesh::get_bounding_box() const noexcept {
	return bounding_box;
}

GET const std::vector<Vertex> & Mesh::get_vertices() const noexcept {
	return vertices;
}

GET const std::vector<Triangle> & Mesh::get_triangles() const noexcept {
	return triangles;
}

GET const std::vector<SimpleVertex> & Mesh::get_physics_vertices() const noexcept {
	return physics_vertices;
}

GET const std::vector<SimpleTriangle> & Mesh::get_physics_triangles() const noexcept {
	return physics_triangles;
}

void Mesh::set_vertices(const std::vector<Vertex> &new_verts, size_t physics_verts_chunk_divisor) {
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

		compile();

		/*vertices.clear();
		for (const SimpleVertex &vert : bounding_box.vertices) {
		vertices.push_back(Vertex(vert.position, { 0, 0 }, { 0, 0, 0 }));
		}*/
	}
}

//ReadObjFileDataOutput Mesh::read_obj_file(const ReadObjFileDataInput &read) {
//	if (!read.contents.empty()) {
//		std::string line;
//
//		std::vector<Vertex> final_verts;
//
//		std::vector<FVector3> vertices;
//		std::vector<FVector2> texcoords;
//		std::vector<FVector3> normals;
//
//		for (std::string line : read.contents) {
//			std::vector<std::string> words = split(line, ' ');
//			if (words.size() > 1) {
//				if (words[0] == "v") {
//					vertices.push_back(FVector3(
//						std::stof(words[1]),
//						std::stof(words[2]),
//						std::stof(words[3])
//					));
//				} else if (words[0] == "vt") {
//					texcoords.push_back(FVector2(
//						std::stof(words[1]),
//						std::stof(words[2])
//					));
//				} else if (words[0] == "vn") {
//					normals.push_back(FVector3(
//						std::stof(words[1]),
//						std::stof(words[2]),
//						std::stof(words[3])
//					));
//				} else if (words[0] == "f") {
//					std::vector<std::string> words_in_face_line(words.begin()+1, words.end());
//					for (std::string vertex : words_in_face_line) {
//						std::vector<std::string> vertex_components = split(vertex, '/');
//
//						if (vertex_components.size() == 3) {
//							final_verts.push_back(Vertex(
//								vertices[std::stoi(vertex_components[0])-read.offsets.vertex_index_offset-1],
//								texcoords[std::stoi(vertex_components[1])-read.offsets.texcoord_index_offset-1],
//								normals[std::stoi(vertex_components[2])-read.offsets.normal_index_offset-1]
//							));
//						} else if (vertex_components.size() < 3) {
//							final_verts.push_back(Vertex(
//								vertices[std::stoi(vertex_components[0])-read.offsets.vertex_index_offset-1],
//								FVector2(0.0f, 0.0f),
//								normals[std::stoi(vertex_components[1])-read.offsets.normal_index_offset-1]
//							));
//						} else {
//							throw std::exception(
//								(" \"" + wstring_to_string(read.file_name) + "\" is formatted incorrectly. A face descriptor has more than 4 specifiers for it's vertex description.").c_str()
//							);
//						}
//					}
//				}
//			}
//		}
//
//		set_vertices(final_verts);
//
//		return ReadObjFileDataOutput(vertices.size(), texcoords.size(), normals.size());
//	}
//	return ReadObjFileDataOutput();
//}