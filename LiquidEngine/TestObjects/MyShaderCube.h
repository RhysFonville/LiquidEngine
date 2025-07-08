#pragma once

#include "../Core/Object.h"
#include "../Components/StaticMeshComponent.h"
#include "../Components/BoundingBoxComponent.h"
#include "../Graphics/Pipeline/GraphicsPipelineRootArgumentContainers.h"

class MyShaderCube : public Object {
public:
	MyShaderCube();

	void post_scene_compile() override;
	void tick(float dt) override;

	void render_editor_gui_section() override;

private:
	StaticMeshComponent* mesh{nullptr};
	BoundingBoxComponent* box{nullptr};

	Object* child{};

	struct s {
		float amplitude;
		float frequency;
		float phase;
		float time;
	};

	RootConstantsContainer<s> rc{s{}};
};
