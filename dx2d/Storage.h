#pragma once

#include <memory>
#include <vector>
#include "Material.h"

class Storage {
public:
	static GET Material & get_material_by_name(std::string name);

	static inline std::vector<Material> materials;
};
