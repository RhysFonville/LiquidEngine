#include "MeshComponent.h"

MeshComponent::MeshComponent(const FVector3 &position, const FVector3 &rotation)
	: Component(Component::Type::MeshComponent, Transform(position, rotation)), material(Material()) { }

MeshComponent::MeshComponent(const Material &material)
	: Component(Component::Type::MeshComponent), material(material) { }

ReadObjFileDataOutput MeshComponent::read_obj_file(const ReadObjFileDataInput &read) {
	if (!read.contents.empty()) {
		mesh_data.vertices.clear();
		mesh_data.indices.clear();

		std::string line;

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
							mesh_data.vertices.push_back(Vertex(
								vertices[std::stoi(vertex_components[0])-read.offsets.vertex_index_offset-1],
								texcoords[std::stoi(vertex_components[1])-read.offsets.texcoord_index_offset-1],
								normals[std::stoi(vertex_components[2])-read.offsets.normal_index_offset-1]
							));
						} else if (vertex_components.size() < 3) {
							mesh_data.vertices.push_back(Vertex(
								vertices[std::stoi(vertex_components[0])-read.offsets.vertex_index_offset-1],
								FVector2(0.0f, 0.0f),
								normals[std::stoi(vertex_components[1])-read.offsets.normal_index_offset-1]
							));
						} else {
							throw (L" \"" + read.file_name + L"\" is formatted incorrectly. A face descriptor has more than 4 specifiers for it's vertex description.").c_str();
						}
					}
				}
			}
		}

		return ReadObjFileDataOutput(vertices.size(), texcoords.size(), normals.size());
	}
	return ReadObjFileDataOutput();
}

void MeshComponent::clean_up() {
	material.clean_up();
}

void MeshComponent::compile() noexcept {
	for (int i = 0; i < mesh_data.vertices.size(); i += 3){
		if (i <= mesh_data.vertices.size()) {
			// Shortcuts for vertices
			FVector3 &v0 = mesh_data.vertices[i+0].position;
			FVector3 &v1 = mesh_data.vertices[i+1].position;
			FVector3 &v2 = mesh_data.vertices[i+2].position;

			// Shortcuts for UVs
			FVector2 &uv0 = mesh_data.vertices[i+0].texcoord;
			FVector2 &uv1 = mesh_data.vertices[i+1].texcoord;
			FVector2 &uv2 = mesh_data.vertices[i+2].texcoord;

			// Edges of the triangle : position delta
			FVector3 deltaPos1 = v1-v0;
			FVector3 deltaPos2 = v2-v0;

			// UV delta
			FVector2 deltaUV1 = uv1-uv0;
			FVector2 deltaUV2 = uv2-uv0;

			float r =     1.0f / (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			FVector3 tangent =   (deltaPos1 * deltaUV2.y - deltaPos2 * deltaUV1.y)*r;
			FVector3 bitangent = (deltaPos2 * deltaUV1.x - deltaPos1 * deltaUV2.x)*r;

			mesh_data.vertices[i+0].tangent = tangent;
			mesh_data.vertices[i+1].tangent = tangent;
			mesh_data.vertices[i+2].tangent = tangent;

			mesh_data.vertices[i+0].bitangent = bitangent;
			mesh_data.vertices[i+1].bitangent = bitangent;
			mesh_data.vertices[i+2].bitangent = bitangent;
		}
	}
}

bool MeshComponent::operator==(const MeshComponent &appearance) const noexcept {
	if (material == appearance.material &&
		mesh_data == appearance.mesh_data) return true; else return false;
}
