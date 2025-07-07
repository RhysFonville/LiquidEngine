#pragma once

#include "../SceneComponents/Object.h"
#include "../Components/StaticMeshComponent.h"
#include "../Components/BoundingBoxComponent.h"
#include "../Graphics/Pipeline/GraphicsPipelineRootArgumentContainers.h"

class MyShaderCube : public Object {
public:
	MyShaderCube();

	void pre_scene_compile() override;
	void post_scene_compile() override;
	void tick(float dt) override;

	void render_editor_gui_section() override;

private:
	std::shared_ptr<StaticMeshComponent> mesh{nullptr};
	std::shared_ptr<BoundingBoxComponent> box{nullptr};

	Object child{};

	struct s {
		float amplitude;
		float frequency;
		float phase;
		float time;
	};

	RootConstantsContainer<s> rc{s{}};
};
