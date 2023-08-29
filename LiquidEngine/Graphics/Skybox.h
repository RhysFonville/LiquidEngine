#pragma once

#include "GraphicsPipeline.h"
#include "Texture.h"

class Skybox {
public:
	Skybox() { }

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const UVector2 &resolution);
	void compile();

	bool has_texture() const noexcept;

	Texture albedo_texture = Texture("Skybox.png");
	Color albedo = Color(0, 0, 0, 255);
	
	GraphicsPipeline pipeline;

private:
	std::shared_ptr<GraphicsPipelineMeshChangeManager> proxy = std::make_shared<GraphicsPipelineMeshChangeManager>();
};
