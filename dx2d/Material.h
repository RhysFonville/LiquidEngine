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

		float ks;
		float kd;
		float ka;
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

	float ks;
	float kd;
	float ka;
	float a;

	//float metallic;
	//float specular = 0.0f;
	//float shininess = 0.0f;
	//float roughness;
	//float anisotropy;
	//float emissive_color;

	std::string pixel_shader_name = "PixelShader.cso";
	std::string vertex_shader_name = "VertexShader.cso";

	std::string name = "Material";

	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout = nullptr;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> vertex_blob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> pixel_blob = nullptr;
};
