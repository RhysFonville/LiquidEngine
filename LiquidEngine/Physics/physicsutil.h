#pragma once

#include "../Utility/globalutil.h"

enum class Unit {
	Meters,						// Distance
	Kilograms,					// Mass
	Newtons,					// Force
	MetersPerSecond,			// Velocity
	MetersPerSecondSquared,		// Acceleration
	Joules						// Kinetic Energy
};

class Force : public FVector3 {
public:
	enum class Type {
		Impulse,
		Constant
	};

	std::string name = "";
	Type type;

	Force(FVector3 vector, std::string name = "",
		Type type = Type::Impulse) : FVector3(vector), name(name), type(type) { }

	bool operator==(const Force &force) const noexcept {
		return (name == force.name && type == force.type &&
			x == force.x && y == force.y && z == force.z);
	}
};

static SimpleBox transform_simple_box(SimpleBox box, const Transform &transform) {
	auto v = transform_simple_vertices(std::vector<SimpleVertex>{box.vertices.begin(), box.vertices.end()}, transform);
	std::copy_n(std::make_move_iterator(v.begin()), box.vertices.size(), box.vertices.begin());
	return box;
}
