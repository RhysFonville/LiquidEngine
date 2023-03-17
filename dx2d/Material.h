#pragma once

#include <d3dcompiler.h>
#include <fstream>
#include "Texture.h"
#include "Throw.h"

class Material {
public:
	__declspec(align(16))
	struct ConstantBufferStruct {
		BOOL has_texture;
		BOOL has_normal_map;

		//XMFLOAT4 diffuse;
		//float specular;
		//float shininess;

		BOOL pad0, pad1;

		XMFLOAT4 ks;
		XMFLOAT4 kd;
		XMFLOAT4 ka;
		
		float a;
	};

public:
	Material(std::string name = "");

	void compile(bool compile_texture = true);
	
	void clean_up();

	void read_mtl_file(std::vector<std::string> contents) noexcept;

	bool operator==(const Material &material) const noexcept;
	operator ConstantBufferStruct() const noexcept;

	Texture texture;
	Texture normal_map;
	//Color diffuse;

	Color ks = Color(255, 255, 255, 255); // Specular
	Color kd = Color(255, 255, 255, 255); // Diffuse
	Color ka = Color(0, 0, 0, 255); // Ambient
	float a = 0.5f; // Shininess

	std::string pixel_shader = "PixelShader.cso";
	std::string vertex_shader = "VertexShader.cso";

	std::string name = "Material";
};
