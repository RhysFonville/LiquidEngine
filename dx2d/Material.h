#pragma once

#include <d3dcompiler.h>
#include <fstream>
//#include "Texture.h"
#include "Throw.h"
#include "GraphicsPipeline.h"

class Material {
public:
	enum class Type {
		Lit,
		Unlit
	};

	Material() { }
	Material(Type type) { }

	void compile();
	
	void clean_up();

	bool operator==(const Material &material) const noexcept;

	std::string vs = "DefaultVertex.hlsl";
	std::string hs = "";
	std::string ds = "";
	std::string gs = "";
	std::string ps = "UnlitPixel.hlsl";

	const Type type = Type::Lit;
};

class UnlitMaterial : public Material {
public:
	UnlitMaterial();
};

class LitMaterial : public Material {
public:
	LitMaterial() { }

	void read_mtl_file(std::vector<std::string> contents) noexcept;

	bool operator==(const LitMaterial &material);

	__declspec(align(16))
		struct ConstantBufferStruct {
		/*BOOL has_texture;
		BOOL has_normal_map;*/

		XMFLOAT4 ks;
		XMFLOAT4 kd;
		XMFLOAT4 ka;

		float a;
	};

private:
	Color ks = Color(255, 255, 255, 255); // Specular
	Color kd = Color(255, 255, 255, 255); // Diffuse
	Color ka = Color(0, 0, 0, 255); // Ambient
	float a = 0.5f; // Shininess

	ConstantBufferStruct cbs;
};
