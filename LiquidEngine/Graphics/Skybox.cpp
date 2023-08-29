#include "Skybox.h"

void Skybox::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const UVector2 &resolution) {
	compile();
	pipeline.compile(device, command_list, sample_desc, resolution);

	pipeline.compilation_signal = false;
}

void Skybox::compile() {
	pipeline.input_assembler.set_proxy(proxy);

	pipeline.input_layout = {
		{ "POSITION",	0,	DXGI_FORMAT_R32G32B32_FLOAT,	0,	0,								D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA, 0 },
		{ "TEXCOORD",	0,	DXGI_FORMAT_R32G32_FLOAT,		0,	D3D12_APPEND_ALIGNED_ELEMENT,	D3D12_INPUT_CLASSIFICATION_PER_VERTEX_DATA,	0 },
	};

	auto rdesc = pipeline.rasterizer.get_desc();
	rdesc.CullMode = D3D12_CULL_MODE_FRONT;
	pipeline.rasterizer.set_desc(rdesc);

	pipeline.vs = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Vertex, "Graphics/SkyboxVS.hlsl");
	pipeline.ps = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Pixel, "Graphics/SkyboxPS.hlsl");

	if (has_texture()) {
		albedo_texture.compile();
	}
	pipeline.root_signature.bind_shader_resource_view(
		albedo_texture.srv,
		D3D12_SHADER_VISIBILITY_PIXEL
	);
	//albedo_texture.srv.srv_desc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURECUBE;

	proxy->add_mesh(Mesh("Shapes/cube.obj"));
}

bool Skybox::has_texture() const noexcept {
	return albedo_texture.exists();
}
