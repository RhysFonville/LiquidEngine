#pragma once

#include <d3dcompiler.h>
#include <fstream>
//#include "Texture.h"
#include "Throw.h"
#include "GraphicsPipeline.h"

class Material {
public:
	__declspec(align(16))
		struct ConstantBufferStruct {
			/*BOOL has_texture;
			BOOL has_normal_map;*/

			XMFLOAT4 ks = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			XMFLOAT4 kd = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);
			XMFLOAT4 ka = XMFLOAT4(0.0f, 0.0f, 0.0f, 0.0f);

			float a = 0.0f;
	};

public:
	Material() { }
	Material(Color ks, Color kd, Color ka, float a)
		: ks(ks), kd(kd), ka(ka), a(a), cbs(*this) { }

	void compile(GraphicsPipeline &pipeline);
	
	void clean_up();

	void read_mtl_file(std::vector<std::string> contents) noexcept;

	void operator=(const Material &material) noexcept;
	bool operator==(const Material &material) const noexcept;

	operator ConstantBufferStruct() const noexcept {
		return ConstantBufferStruct({
			color_to_XMFLOAT4(ks),
			color_to_XMFLOAT4(kd),
			color_to_XMFLOAT4(ka)
		});
	}

private:
	std::string vs = "DefaultVertex.hlsl";
	std::string hs = "";
	std::string ds = "";
	std::string gs = "";
	std::string ps = "UnlitPixel.hlsl";

	Color ks = Color(255, 255, 255, 255); // Specular
	Color kd = Color(255, 255, 255, 255); // Diffuse
	Color ka = Color(0, 0, 0, 255); // Ambient
	float a = 0.5f; // Shininess

	ConstantBufferStruct cbs;
	GraphicsPipeline::RootSignature::ConstantBuffer cb;
};

__declspec(align(16))
struct PerObjectPSCB { // b3
	Material::ConstantBufferStruct material;
};
