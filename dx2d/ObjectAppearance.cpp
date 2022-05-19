#include "ObjectAppearance.h"

ObjectAppearance::ObjectAppearance() { }
ObjectAppearance::ObjectAppearance(std::string pixel, std::string vertex)
	: pixel_shader(pixel), vertex_shader(vertex) { }
