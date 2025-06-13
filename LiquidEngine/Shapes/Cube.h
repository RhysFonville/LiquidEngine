#pragma once

#include "../SceneComponents/Object.h"
#include "../Components/StaticMeshComponent.h"

namespace DefaultShapes {

class Cube : public Object {
public:
	Cube();

	void pre_scene_compile() override;

private:
	std::shared_ptr<StaticMeshComponent> mesh;
};

};
