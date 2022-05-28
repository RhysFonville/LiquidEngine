#pragma once

#include <d3dcompiler.h>
#include "Texture.h"
#include "Throw.h"

class Material {
public:
	Material();

	Texture texture;

	Microsoft::WRL::ComPtr<ID3D11InputLayout> input_layout = nullptr;

	Microsoft::WRL::ComPtr<ID3D11VertexShader> vertex_shader = nullptr;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> pixel_shader = nullptr;

	Microsoft::WRL::ComPtr<ID3DBlob> vertex_blob = nullptr;
	Microsoft::WRL::ComPtr<ID3DBlob> pixel_blob = nullptr;

	std::string pixel_shader_name = "PixelShader.cso";
	std::string vertex_shader_name = "VertexShader.cso";
};

