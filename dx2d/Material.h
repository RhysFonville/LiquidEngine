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

		float pad;

		XMFLOAT4 ks;
		XMFLOAT4 kd;
		XMFLOAT4 ka;

		float pad1;
		
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
	Color ka = Color(0, 0, 0, 0); // Ambient
	float a = 0.5f; // Shininess

	std::string pixel_shader_name = "PixelShader.cso";
	std::string vertex_shader_name = "VertexShader.cso";

	std::string name = "Material";

	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout = nullptr;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> vertex_blob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> pixel_blob = nullptr;
};
