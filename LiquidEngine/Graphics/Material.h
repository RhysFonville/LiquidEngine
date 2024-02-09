#pragma once

#include <d3dcompiler.h>
#include <fstream>
#include "Texture.h"

/**
 * Material data for rendering.
 */
class Material {
public:
	Material() { }

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc, const UVector2 &resolution);
	void compile();
	
	void clean_up();

	void set_data(const std::string &file);
	void set_data(const Material &material);

	bool has_texture() const noexcept;
	bool has_normal_map() const noexcept;

	void operator=(const Material &material) noexcept;
	bool operator==(const Material &material) const noexcept;

	const Texture & get_albedo_texture() const { return albedo_texture; }
	void set_albedo_texture(std::string texture) { albedo_texture.set_texture(texture); }

	const Texture & get_normal_map() const { return normal_map; }
	void set_normal_map(std::string texture) { normal_map.set_texture(texture); }

	Color get_albedo() const { return albedo; }
	void set_albedo(const Color &albedo) { this->albedo = albedo; }
	
	Color get_specular() const { return specular; }
	void set_specular(const Color &specular) { this->specular = specular; }

	Color get_ambient() const { return ambient; }
	void set_ambient(const Color &ambient) { this->ambient = ambient; }

	float get_shininess() const { return shininess; }
	void set_shininess(const float &shininess) { this->shininess = shininess; }

	GraphicsPipeline pipeline;

private:
	std::string vs = "Graphics/DefaultVertex.hlsl";
	std::string hs = "";
	std::string ds = "";
	std::string gs = "";
	std::string ps = "Graphics/LitPixel.hlsl";

	Texture albedo_texture = Texture();
	Texture normal_map = Texture();
	Color albedo = Color(100, 100, 100, 100); // kd
	Color specular = Color(5, 5, 5, 255); // ks
	Color ambient = Color(0, 0, 0, 255); // ka
	float shininess = 0.5f; // a
};
