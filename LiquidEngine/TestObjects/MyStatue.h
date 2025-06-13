#pragma once

#include "../SceneComponents/Object.h"
#include "../Components/StaticMeshComponent.h"

class MyStatue : public Object {
public:
	MyStatue();

	void pre_scene_compile() override;
	void tick(float dt) override;

private:
	std::shared_ptr<StaticMeshComponent> mesh;

	int timer{0};
	bool reg_tex{false};
};

