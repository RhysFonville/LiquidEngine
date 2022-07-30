#pragma once

#include <filesystem>
#include <wrl.h>
#include <d3d11.h>
#include "WICTextureLoader.h"
#include "Throw.h"
#include "globalutil.h"

class Texture {
public:
	Texture();
	Texture(std::string file_name);

	void clean_up();

	bool operator==(const Texture &texture) const noexcept;

	Microsoft::WRL::ComPtr<ID3D11Resource> texture = nullptr;
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> texture_view = nullptr;
	//Microsoft::WRL::ComPtr<ID3D11SamplerState> sampler_state = nullptr;

	std::string file_name = "";
};
