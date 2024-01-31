#pragma once

#include <DirectXTex.h>
#include "GraphicsPipeline.h"

class Texture {
public:
	Texture() { }
	Texture(const std::string &file);

	void compile();

	bool operator==(const Texture &texture) const noexcept;

	void set_texture(const std::string &file);
	
	GET std::string get_file() const noexcept { return file; }

	bool exists() const noexcept { return fs::exists(file); }

	static void load_texture(const std::string &file, DirectX::ScratchImage &image, DirectX::ScratchImage &mip_chain);

	// Content of the texture resource
	DirectX::ScratchImage image;
	DirectX::ScratchImage mip_chain;

	GraphicsPipeline::RootSignature::ShaderResourceView srv;

private:
	std::string file = "";
};
