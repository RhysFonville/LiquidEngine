#pragma once

#include <string>
#include <vector>
#include <Windows.h>
#include "Material.h"

struct Color {
	UCHAR r = 255, g = 255, b = 255, a = 255;
};

struct FVector2 {
	float x = 0;
	float y = 0;

	FVector2() : x(0), y(0) {}
	FVector2(float x, float y) : x(x), y(y) {}

	FVector2 operator+(FVector2 vector) {
		FVector2 ret = vector;
		ret.x += x;
		ret.y += y;
		return ret;
	}

	void operator+=(FVector2 vector) {
		x += vector.x;
		y += vector.y;
	}
};

struct FVector3 {
	float x = 0;
	float y = 0;
	float z = 0;

	FVector3() : x(0), y(0), z(0) {}
	FVector3(float x, float y, float z) : x(x), y(y), z(z) {}

	FVector3 operator+(FVector3 vector) {
		FVector3 ret = vector;
		ret.x += x;
		ret.y += y;
		ret.z += z;
		return ret;
	}

	void operator+=(FVector3 vector) {
		x += vector.x;
		y += vector.y;
		z += vector.z;
	}
};

using FPosition = FVector3;
using FPosition3 = FVector3;
using FPosition2 = FVector2;

struct Vertex {
	FPosition3 position = FPosition3();
	FPosition2 tex_coord = FPosition2();

	Vertex(float x, float y, float z) : position({ x, y, z }) {
	}

	Vertex(float x, float y, float z, float u, float v)
		: position({ x, y, z }), tex_coord({ u, v }) { }
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<USHORT> indices;
};

class ObjectAppearance {
public:
	ObjectAppearance();
	ObjectAppearance(std::string pixel, std::string vertex);

	Material material;

	Mesh mesh = {
		 {
			/*{-1.0f, -1.0f, -1.0f, 255, 0, 0, 255},
			{ 1.0f, -1.0f, -1.0f, 0, 255, 0, 255 },
			{ -1.0f, 1.0f, -1.0f, 0, 0, 255, 255 },
			{ 1.0f, 1.0f, -1.0f, 255, 0, 255, 255 },
			{ -1.0f, -1.0f, 1.0f, 255, 255, 0, 255 },
			{ 1.0f, -1.0f, 1.0f, 0, 255, 255, 255 },
			{ -1.0f, 1.0f, 1.0f, 255, 0, 0, 255 },
			{ 1.0f, 1.0f, 1.0f, 0, 255, 0, 255 }*/
			// Front Face
			Vertex(-1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
			Vertex(-1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
			Vertex( 1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
			Vertex( 1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

			// Back Face
			Vertex(-1.0f, -1.0f, 1.0f, 1.0f, 1.0f),
			Vertex( 1.0f, -1.0f, 1.0f, 0.0f, 1.0f),
			Vertex( 1.0f,  1.0f, 1.0f, 0.0f, 0.0f),
			Vertex(-1.0f,  1.0f, 1.0f, 1.0f, 0.0f),

			// Top Face
			Vertex(-1.0f, 1.0f, -1.0f, 0.0f, 1.0f),
			Vertex(-1.0f, 1.0f,  1.0f, 0.0f, 0.0f),
			Vertex( 1.0f, 1.0f,  1.0f, 1.0f, 0.0f),
			Vertex( 1.0f, 1.0f, -1.0f, 1.0f, 1.0f),

			// Bottom Face
			Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),
			Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
			Vertex( 1.0f, -1.0f,  1.0f, 0.0f, 0.0f),
			Vertex(-1.0f, -1.0f,  1.0f, 1.0f, 0.0f),

			// Left Face
			Vertex(-1.0f, -1.0f,  1.0f, 0.0f, 1.0f),
			Vertex(-1.0f,  1.0f,  1.0f, 0.0f, 0.0f),
			Vertex(-1.0f,  1.0f, -1.0f, 1.0f, 0.0f),
			Vertex(-1.0f, -1.0f, -1.0f, 1.0f, 1.0f),

			// Right Face
			Vertex( 1.0f, -1.0f, -1.0f, 0.0f, 1.0f),
			Vertex( 1.0f,  1.0f, -1.0f, 0.0f, 0.0f),
			Vertex( 1.0f,  1.0f,  1.0f, 1.0f, 0.0f),
			Vertex( 1.0f, -1.0f,  1.0f, 1.0f, 1.0f),
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
};

