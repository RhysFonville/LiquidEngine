#include "MeshComponent.h"

MeshComponent::MeshComponent(const FPosition3 &position, const FRotation3 &rotation)
	: Component(Component::Type::MeshComponent, Transform(position, rotation)), material(Material()) { }

MeshComponent::MeshComponent(const Material &material)
	: Component(Component::Type::MeshComponent), material(material) { }

ReadObjFileDataOutput MeshComponent::read_obj_file(const ReadObjFileDataInput &read) noexcept {
	if (!read.contents.empty()) {
		mesh_data.vertices.clear();
		mesh_data.indices.clear();

		std::string line;

		std::vector<FPosition> vertices;
		std::vector<TexCoord> texcoords;
		std::vector<Normal> normals;

		for (std::string line : read.contents) {
			std::vector<std::string> words = split(line, ' ');
			if (words.size() > 1) {
				if (words[0] == "v") {
					vertices.push_back(FPosition(
						std::stof(words[1]),
						std::stof(words[2]),
						std::stof(words[3])
					));
				} else if (words[0] == "vt") {
					texcoords.push_back(TexCoord(
						std::stof(words[1]),
						std::stof(words[2])
					));
				} else if (words[0] == "vn") {
					normals.push_back(Normal(
						std::stof(words[1]),
						std::stof(words[2]),
						std::stof(words[3])
					));
				} else if (words[0] == "f") {
					std::vector<std::string> words_in_face_line(words.begin()+1, words.end());
					for (std::string vertex : words_in_face_line) {
						std::vector<std::string> vertex_components = split(vertex, '/');

						mesh_data.vertices.push_back(Vertex(
							vertices[std::stoi(vertex_components[0])-read.offsets.vertex_index_offset-1],
							texcoords[std::stoi(vertex_components[1])-read.offsets.texcoord_index_offset-1],
							normals[std::stoi(vertex_components[2])-read.offsets.normal_index_offset-1]
						));
					}
				}
			}
		}

		return ReadObjFileDataOutput(vertices.size(), texcoords.size(), normals.size());
	}
	return ReadObjFileDataOutput();
}

bool MeshComponent::operator==(const MeshComponent &appearance) const noexcept {
	if (material == appearance.material &&
		mesh_data == appearance.mesh_data) return true; else return false;
}

void MeshComponent::clean_up() {
	material.clean_up();
}
