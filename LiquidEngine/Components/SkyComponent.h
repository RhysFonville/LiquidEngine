#pragma once

#include "../Graphics/GraphicsPipeline.h"
#include "../Graphics/Texture.h"
#include "GraphicsComponent.h"

/**
* Component for a sky.
*/
class SkyComponent : public GraphicsComponent {
public:
	SkyComponent();

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc, const UVector2 &resolution);
	void compile() override;

	bool has_texture() const noexcept;

	const Texture & get_albedo_texture() const { return albedo_texture; }
	void set_albedo_texture(std::string texture) { albedo_texture.set_texture(texture); changed = true; }

	Color get_albedo() const { return albedo; }
	void set_albedo(const Color &new_albedo) { albedo = new_albedo; changed = true; }

	static Mesh create_sphere(UINT lat_lines, UINT long_lines);

	static const Type component_type = Type::SkyComponent;

	GraphicsPipeline pipeline;

private:
	Texture albedo_texture = Texture();
	Color albedo = Color(0, 0, 0, 255);
	
	std::shared_ptr<GraphicsPipelineMeshChange::Manager> proxy = std::make_shared<GraphicsPipelineMeshChange::Manager>();
};
