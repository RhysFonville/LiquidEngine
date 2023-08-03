#pragma once

#include <d3dcompiler.h>
#include <fstream>
//#include "Texture.h"
#include "../Throw.h"
#include "GraphicsPipeline.h"

class Material {
public:
	Material() { }
	Material(Color ks, Color kd, Color ka, float a)
		: data({ ks, kd, ka, a }) { }

	void compile(ComPtr<ID3D12Device> &device, ComPtr<ID3D12GraphicsCommandList> &command_list,
		const DXGI_SAMPLE_DESC &sample_desc, const UVector2 &resolution);
	
	void clean_up();

	void read_mtl_file(std::vector<std::string> contents) noexcept;

	void operator=(const Material &material) noexcept;
	bool operator==(const Material &material) const noexcept;

	//__declspec(align(256))
	struct MaterialData {
		/*BOOL has_texture;
		BOOL has_normal_map;*/

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
	std::string vs = "DefaultVertex.hlsl";
	std::string hs = "";
	std::string ds = "";
	std::string gs = "";
	std::string ps = "LitPixel.hlsl";

	
};
