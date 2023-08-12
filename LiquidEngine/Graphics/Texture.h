#pragma once

#include "WICTextureLoader.h"
#include "../Throw.h"
#include "../globalutil.h"

class Texture {
public:
	Texture();
	Texture(std::string file_name);

	void clean_up();

	bool operator==(const Texture &texture) const noexcept;

	/*ComPtr<ID3D11Resource> texture = nullptr;
	ComPtr<ID3D11ShaderResourceView> texture_view = nullptr;
	ComPtr<ID3D11SamplerState> sampler_state = nullptr;*/

	std::string file_name = "";
};
