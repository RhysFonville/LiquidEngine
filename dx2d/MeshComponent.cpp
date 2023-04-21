#include "MeshComponent.h"

MeshComponent::MeshComponent(const FVector3 &position, const FVector3 &rotation)
	: Component(Component::Type::MeshComponent, Transform(position, rotation)) { }

ReadObjFileDataOutput MeshComponent::read_obj_file(const ReadObjFileDataInput &read) {
	if (!read.contents.empty()) {
		std::string line;

		std::vector<Vertex> final_verts;

		std::vector<FVector3> vertices;
		std::vector<FVector2> texcoords;
		std::vector<FVector3> normals;

		for (std::string line : read.contents) {
			std::vector<std::string> words = split(line, ' ');
			if (words.size() > 1) {
				if (words[0] == "v") {
					vertices.push_back(FVector3(
						std::stof(words[1]),
						std::stof(words[2]),
						std::stof(words[3])
					));
				} else if (words[0] == "vt") {
					texcoords.push_back(FVector2(
						std::stof(words[1]),
						std::stof(words[2])
					));
				} else if (words[0] == "vn") {
					normals.push_back(FVector3(
						std::stof(words[1]),
						std::stof(words[2]),
						std::stof(words[3])
					));
				} else if (words[0] == "f") {
					std::vector<std::string> words_in_face_line(words.begin()+1, words.end());
					for (std::string vertex : words_in_face_line) {
						std::vector<std::string> vertex_components = split(vertex, '/');

						if (vertex_components.size() == 3) {
							final_verts.push_back(Vertex(
								vertices[std::stoi(vertex_components[0])-read.offsets.vertex_index_offset-1],
								texcoords[std::stoi(vertex_components[1])-read.offsets.texcoord_index_offset-1],
								normals[std::stoi(vertex_components[2])-read.offsets.normal_index_offset-1]
							));
						} else if (vertex_components.size() < 3) {
							final_verts.push_back(Vertex(
								vertices[std::stoi(vertex_components[0])-read.offsets.vertex_index_offset-1],
								FVector2(0.0f, 0.0f),
								normals[std::stoi(vertex_components[1])-read.offsets.normal_index_offset-1]
							));
						} else {
							throw std::exception(
								(" \"" + wstring_to_string(read.file_name) + "\" is formatted incorrectly. A face descriptor has more than 4 specifiers for it's vertex description.").c_str()
							);
						}
					}
				}
			}
		}

		mesh_data.set_vertices(final_verts);

		return ReadObjFileDataOutput(vertices.size(), texcoords.size(), normals.size());
	}
	return ReadObjFileDataOutput();
}

void MeshComponent::compile() noexcept {
	std::vector<Vertex> verts = mesh_data.get_vertices();
	for (size_t i = 0; i < verts.size(); i += 3){
		if (i <= verts.size()) {

			// Shortcuts for vertices
			FVector3 v0 = verts[i+0].position;
			FVector3 v1 = verts[i+1].position;
			FVector3 v2 = verts[i+2].position;

			// Shortcuts for UVs
			FVector2 uv0 = verts[i+0].texcoord;
			FVector2 uv1 = verts[i+1].texcoord;
			FVector2 uv2 = verts[i+2].texcoord;

			// Edges of the triangle : position delta
			FVector3 deltaPos1 = v1-v0;
			FVector3 deltaPos2 = v2-v0;

			// UV delta
			FVector2 deltaUV1 = uv1-uv0;
			FVector2 deltaUV2 = uv2-uv0;

			float r =     1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			FVector3 tangent =   (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			//FVector3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

			verts[i+0].tangent = tangent;
			verts[i+1].tangent = tangent;
			verts[i+2].tangent = tangent;

			/*verts[i+0].bitangent = bitangent;
			verts[i+1].bitangent = bitangent;
			verts[i+2].bitangent = bitangent;*/
		}
	}

	mesh_data.set_vertices(verts);
	mesh_data.indices = std::vector<UINT>();
}

bool MeshComponent::operator==(const MeshComponent &mesh) const noexcept {
	return ((Component*)this == (Component*)&mesh &&
			mesh_data == mesh.mesh_data);
}

void MeshComponent::operator=(const MeshComponent &component) noexcept {
	mesh_data = component.mesh_data;
}
