#pragma once

#include "../Core/Object.h"
#include "../Components/StaticMeshComponent.h"

namespace DefaultShapes {

class Cube : public Object {
public:
	Cube();

private:
	StaticMeshComponent* mesh{nullptr};
};

};
