#pragma once

#include "DirectXTex.h"
#include "../Throw.h"
#include "../globalutil.h"
#include "GraphicsPipeline.h"

class Texture {
public:
	Texture() { }
	Texture(const std::string &file);

	void compile();
	void clean_up();

	bool operator==(const Texture &texture) const noexcept;

	void set_texture(const std::string &file);
	
	GET std::string get_file() const noexcept { return file; }

	bool exists() const noexcept { return fs::exists(file); }

	static void load_texture(const std::string &file, DirectX::TexMetadata &metadata, DirectX::ScratchImage &scratch_image) {
		DirectX::LoadFromWICFile(
			string_to_wstring(file).c_str(),
			WIC_FLAGS_FORCE_RGB,
			&metadata,
			scratch_image
		);
	}

	// Informations about the texture resource
	DirectX::TexMetadata metadata;

	// Content of the texture resource
	DirectX::ScratchImage scratch_image;

	GraphicsPipeline::RootSignature::ShaderResourceView srv;

private:
	std::string file;
};
