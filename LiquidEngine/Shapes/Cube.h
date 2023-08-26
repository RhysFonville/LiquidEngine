#pragma once

#include "../Object.h"
#include "../Components/StaticMeshComponent.h"

namespace DefaultShapes {

class Cube : public Object {
public:
	Cube();

	void on_start() override;

private:
	std::shared_ptr<StaticMeshComponent> mesh;
};

};
