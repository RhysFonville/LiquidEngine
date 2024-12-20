#pragma once

#include "../Object.h"

class MyShaderCube : public Object {
public:
	MyShaderCube();

	void pre_scene_compile() override;
	void tick(float dt) override;

	void render_editor_gui_section() override;

private:
	std::shared_ptr<StaticMeshComponent> mesh{nullptr};
	std::shared_ptr<BoundingBoxComponent> box{nullptr};

	struct s {
		float time{0.0f};
		float intensity{1.0f};
	};

	RootConstantsContainer<s> rc{s{}};
};
