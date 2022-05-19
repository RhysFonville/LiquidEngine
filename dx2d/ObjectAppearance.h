#pragma once

#include <string>
#include <vector>
#include <Windows.h>

struct Color {
	UCHAR r = 255, g = 255, b = 255, a = 255;
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
		return ret;
	}

	void operator+=(FVector3 vector) {
		x += vector.x;
		y += vector.y;
	}
};

using FPosition = FVector3;

struct Vertex {
	FPosition position = FPosition();
	Color color = Color();

	Vertex(float x, float y, float z)
		: position({ x, y, z }), color({ 255, 255, 255, 255 }) {
	}

	Vertex(float x, float y, float z, UCHAR r, UCHAR g, UCHAR b, UCHAR a)
		: position({ x, y, z }), color({ r, g, b, a }) { }
};

struct Mesh {
	std::vector<Vertex> vertices;
	std::vector<USHORT> indices;
};

class ObjectAppearance {
public:
	ObjectAppearance();
	ObjectAppearance(std::string pixel, std::string vertex);

	std::string pixel_shader = "PixelShader.cso";
	std::string vertex_shader = "VertexShader.cso";
	Mesh mesh = {
		 {
			{ -1.0f, -1.0f, -1.0f, 255, 0, 0, 255 },
			{ 1.0f, -1.0f, -1.0f, 0, 255, 0, 255 },
			{ -1.0f, 1.0f, -1.0f, 0, 0, 255, 255 },
			{ 1.0f, 1.0f, -1.0f, 255, 0, 255, 255 },
			{ -1.0f, -1.0f, 1.0f, 255, 255, 0, 255 },
			{ 1.0f, -1.0f, 1.0f, 0, 255, 255, 255 },
			{ -1.0f, 1.0f, 1.0f, 255, 0, 0, 255 },
			{ 1.0f, 1.0f, 1.0f, 0, 255, 0, 255 },
		},
		{
			0, 2, 1, 2, 3, 1,
			1, 3, 5, 3, 7, 5,
			2, 6, 3, 3, 6, 7,
			4, 5, 7, 4, 7, 6,
			0, 4, 2, 2, 4, 6,
			0, 1, 4, 1, 5, 4,
		}
	};
};

