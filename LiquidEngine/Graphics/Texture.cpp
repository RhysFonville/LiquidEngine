#include "Texture.h"

Texture::Texture(const std::string &file) {
	set_texture(file);
}

void Texture::compile() {
	load_texture(file, metadata, scratch_image, mip_chain);
}

void Texture::clean_up() {
	/*COMPTR_RELEASE(texture);
	COMPTR_RELEASE(texture_view);*/
	//COMPTR_RELEASE(sampler_state);
}

bool Texture::operator==(const Texture &texture) const noexcept {
	return (file == texture.file);
}

void Texture::set_texture(const std::string &file) {
	this->file = file;
	if (exists()) {
		compile();
		srv = GraphicsPipeline::RootSignature::ShaderResourceView(metadata, scratch_image, mip_chain);
	}
}

void Texture::load_texture(const std::string &file, DirectX::TexMetadata &metadata, DirectX::ScratchImage &scratch_image, DirectX::ScratchImage &mip_chain) {
	if (fs::exists(file)) {
		std::string type = to_lower(file.substr(file.find('.')+1));
		if (type == "png" || type == "jpg" || type == "jpeg" || type == "bmp" || type == "gif" || type == "tiff" || type == "jpeg") {
			HPEW(DirectX::LoadFromWICFile(
				string_to_wstring(file).c_str(),
				WIC_FLAGS_NONE,
				&metadata,
				scratch_image
			));
			HPEW(DirectX::GenerateMipMaps(
				*scratch_image.GetImages(),
				DirectX::TEX_FILTER_BOX,
				0,
				mip_chain
			));
		} else {
			throw std::exception("Unsupported file type when loading texture.");
		}
	}
}
