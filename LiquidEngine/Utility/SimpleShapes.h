#pragma once

#include <array>
#include "Vectors.h"

struct SimpleVertex {
	FVector3 position = FVector3();

	SimpleVertex() { }
	SimpleVertex(float x, float y, float z) : position(FVector3(x, y, z)) { }
	SimpleVertex(FVector3 position)
		: position(position) { }

	bool operator==(const SimpleVertex &vertex) const noexcept {
		return (position == vertex.position);
	}

	bool operator!=(const SimpleVertex &vertex) const noexcept {
		return (position != vertex.position);
	}
};

struct SimpleTriangle {
	SimpleVertex first;
	SimpleVertex second;
	SimpleVertex third;

	bool contains(const SimpleVertex &vertex) {
		return (first == vertex || second == vertex || third == vertex);
	}
};

struct SimpleBox {
	std::array<SimpleVertex, 8> vertices = {
		SimpleVertex(0.0f, 0.0f, 0.0f),
		SimpleVertex(0.0f, 0.0f, 1.0f),
		SimpleVertex(0.0f, 1.0f, 0.0f),
		SimpleVertex(0.0f, 1.0f, 1.0f),
		SimpleVertex(1.0f, 0.0f, 0.0f),
		SimpleVertex(1.0f, 0.0f, 1.0f),
		SimpleVertex(1.0f, 1.0f, 0.0f),
		SimpleVertex(1.0f, 1.0f, 1.0f)
	};

	SimpleBox() { }
	SimpleBox(const std::vector<SimpleVertex> &verts) {
		auto x_extremes = std::minmax_element(verts.begin(), verts.end(),
			[](const SimpleVertex &lhs, const SimpleVertex &rhs) {
			return lhs.position.x < rhs.position.x;
		});

		auto y_extremes = std::minmax_element(verts.begin(), verts.end(),
			[](const SimpleVertex &lhs, const SimpleVertex &rhs) {
			return lhs.position.y < rhs.position.y;
		});

		auto z_extremes = std::minmax_element(verts.begin(), verts.end(),
			[](const SimpleVertex &lhs, const SimpleVertex &rhs) {
			return lhs.position.z < rhs.position.z;
		});

		SimpleVertex v1(x_extremes.first->position.x,
			y_extremes.first->position.y,
			z_extremes.first->position.z
		);
		SimpleVertex v2(x_extremes.first->position.x,
			y_extremes.second->position.y,
			z_extremes.first->position.z
		);
		SimpleVertex v3(x_extremes.second->position.x,
			y_extremes.second->position.y,
			z_extremes.first->position.z
		);
		SimpleVertex v4(x_extremes.second->position.x,
			y_extremes.first->position.y,
			z_extremes.first->position.z
		);
		SimpleVertex v5(x_extremes.first->position.x,
			y_extremes.first->position.y,
			z_extremes.second->position.z
		);
		SimpleVertex v6(x_extremes.first->position.x,
			y_extremes.second->position.y,
			z_extremes.second->position.z
		);
		SimpleVertex v7(x_extremes.second->position.x,
			y_extremes.second->position.y,
			z_extremes.second->position.z
		);
		SimpleVertex v8(x_extremes.second->position.x,
			y_extremes.first->position.y,
			z_extremes.second->position.z
		);

		vertices = { v1, v2, v3, v4, v5, v6, v7, v8 };
	}

	std::vector<SimpleTriangle> split_into_triangles() {
		return std::vector<SimpleTriangle>({
			/*FRONT*/	SimpleTriangle({ vertices[0], vertices[1], vertices[2] }),
			SimpleTriangle({ vertices[2], vertices[3], vertices[0] }),
			/*BOTTOM*/	SimpleTriangle({ vertices[7], vertices[4], vertices[0] }),
			SimpleTriangle({ vertices[0], vertices[3], vertices[7] }),
			/*BACK*/	SimpleTriangle({ vertices[6], vertices[5], vertices[4] }),
			SimpleTriangle({ vertices[4], vertices[7], vertices[6] }),
			/*LEFT*/	SimpleTriangle({ vertices[5], vertices[1], vertices[0] }),
			SimpleTriangle({ vertices[0], vertices[4], vertices[5] }),
			/*RIGHT*/	SimpleTriangle({ vertices[2], vertices[6], vertices[7] }),
			SimpleTriangle({ vertices[7], vertices[3], vertices[2] }),
			/*TOP*/		SimpleTriangle({ vertices[5], vertices[6], vertices[2] }),
			SimpleTriangle({ vertices[2], vertices[1], vertices[5] })
			});
	}
};

struct Line {
	FVector3 p1, p2;
};

struct Segment {
	FVector3 p1, p2;
};

struct Ray {
	FVector3 origin;
	FVector3 direction;
};

static std::vector<SimpleTriangle> split_into_simple_triangles(const std::vector<SimpleVertex> &verts) noexcept {
	std::vector<SimpleTriangle> ret;

	for (size_t i = 0; i < verts.size(); i += 3){
		if (i + 2 < verts.size()) {
			ret.push_back({
				verts[i+0],
				verts[i+1],
				verts[i+2]
				});
		}
	}

	return ret;
}
