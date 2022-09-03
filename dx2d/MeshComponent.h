#pragma once

#include <string>
#include <vector>
#include <Windows.h>
#include <fstream>
#include "Material.h"
#include "globalutil.h"
#include "Component.h"

using TexCoord = FPosition2;
using FTexCoord = TexCoord;

struct Vertex {
	FPosition position = FPosition();
	TexCoord texcoord = TexCoord();
	Normal normal = Normal();
	Tangent tangent = Tangent();
	//Bitangent bitangent = Bitangent();

	Vertex(float x, float y, float z) : position(x, y, z) { }

	Vertex(float x, float y, float z, float u, float v)
		: position(x, y, z), texcoord(u, v) { }

	Vertex(float x,  float y,  float z,
		   float u,  float v,
		   float nx, float ny, float nz)
		: position(x, y, z), texcoord(u, v), normal(nx, ny, nz) { }

	Vertex(FPosition position, TexCoord texcoord, Normal normal)
		: position(position), texcoord(texcoord), normal(normal) { }

	bool operator==(const Vertex &vertex) const noexcept {
		if (position == vertex.position &&
			texcoord == vertex.texcoord)
			return true;
		else
			return false;
	}

	bool operator!=(const Vertex &vertex) const noexcept {
		if (position != vertex.position &&
			texcoord != vertex.texcoord)
			return true;
		else
			return false;
	}
};

struct MeshData {
	std::vector<Vertex> vertices;
	std::vector<USHORT> indices;

	bool operator==(const MeshData &mesh) const noexcept {
		if (vertices == mesh.vertices &&
			indices == mesh.indices)
			return true;
		else
			return false;
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

	ReadObjFileDataOutput read_obj_file(const ReadObjFileDataInput &read) noexcept;

	void clean_up() override;

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
