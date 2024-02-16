#pragma once

#include "../Object.h"

class MyStatue : public Object {
public:
	MyStatue();

	void on_start() override;
	void tick(float dt) override;

private:
	std::shared_ptr<StaticMeshComponent> mesh;

	int timer{0};
	bool reg_tex{false};
};

