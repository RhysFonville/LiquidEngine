#pragma once

#include <d3dcompiler.h>
#include <fstream>
#include "Texture.h"
#include "../Throw.h"

class Material {
public:
	Material() { }
	Material(Color ks, Color kd, Color ka, float a)
		: data({ true, Texture("texture.jpg"), ks, kd, ka, a }) { }

	void compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const UVector2 &resolution);
	void compile();
	
	void clean_up();

	void read_mtl_file(std::vector<std::string> contents) noexcept;

	void operator=(const Material &material) noexcept;
	bool operator==(const Material &material) const noexcept;

	//__declspec(align(256))
	struct MaterialData {
		bool has_texture = true;
		//BOOL has_normal_map;

		Texture diffuse_texture = Texture("texture.jpg");

		Color ks = Color(0, 0, 0, 255); // Specular
		Color kd = Color(255, 255, 255, 255); // Diffuse
		Color ka = Color(0, 0, 0, 255); // Ambient
		float a = 0.5f; // Shininess

		bool operator==(const MaterialData &data) const noexcept {
			return (
				ks == data.ks &&
				kd == data.kd &&
				ka == data.ka &&
				a == data.a
			);
		}
	} data;

	GraphicsPipeline pipeline;

private:
	std::string vs = "Graphics/DefaultVertex.hlsl";
	std::string hs = "";
	std::string ds = "";
	std::string gs = "";
	std::string ps = "Graphics/LitPixel.hlsl";
};
