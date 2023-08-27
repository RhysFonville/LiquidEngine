#pragma once

#include <d3dcompiler.h>
#include <fstream>
#include "Texture.h"
#include "../Throw.h"

class Material {
public:
	Material() { }

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const UVector2 &resolution);
	void compile();
	
	void clean_up();

	void set_data(const std::string &file);

	bool has_texture() const noexcept;
	bool has_normal_map() const noexcept;

	void operator=(const Material &material) noexcept;
	bool operator==(const Material &material) const noexcept;

	Texture diffuse_texture = Texture();
	Texture normal_map = Texture();
	Color specular = Color(0, 0, 0, 255); // ks
	Color diffuse = Color(255, 255, 255, 255); // kd
	Color ambient = Color(0, 0, 0, 255); // ka
	float shininess = 0.5f; // a

	GraphicsPipeline pipeline;

private:
	std::string vs = "Graphics/DefaultVertex.hlsl";
	std::string hs = "";
	std::string ds = "";
	std::string gs = "";
	std::string ps = "Graphics/LitPixel.hlsl";

	bool texture_exists(const Texture &texture) const noexcept {
		return (texture.get_file() != "" &&
			(texture.metadata.width != 0 &&
			texture.metadata.height != 0));
	}
};
