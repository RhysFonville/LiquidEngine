#pragma once

#include "../Core/Object.h"
#include "../Components/StaticMeshComponent.h"

class MyStatue : public Object {
public:
	MyStatue();

	void tick(float dt) override;

private:
	StaticMeshComponent* mesh;

	int timer{0};
	bool reg_tex{false};
};

