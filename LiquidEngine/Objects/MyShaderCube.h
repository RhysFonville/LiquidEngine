#pragma once

#include "../Object.h"

class MyShaderCube : public Object {
public:
	MyShaderCube();

	void pre_scene_compile() override;
	void tick(float dt) override;

private:
	std::shared_ptr<StaticMeshComponent> mesh{nullptr};
	std::shared_ptr<BoundingBoxComponent> box{nullptr};

	GraphicsPipeline::RootSignature::RootConstantsContainer<float> time = 0.0f;
};

