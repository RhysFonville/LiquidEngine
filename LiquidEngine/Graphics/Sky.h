#pragma once

#include "GraphicsPipeline.h"
#include "Texture.h"

class Sky {
public:
	Sky() { }

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc, const UVector2 &resolution);
	void compile();

	bool has_texture() const noexcept;

	static Mesh create_sphere(UINT lat_lines, UINT long_lines);

	Texture albedo_texture = Texture();
	Color albedo = Color(255, 0, 0, 255);
	
	GraphicsPipeline pipeline;

private:
	std::shared_ptr<GraphicsPipelineMeshChangeManager> proxy = std::make_shared<GraphicsPipelineMeshChangeManager>();
};
