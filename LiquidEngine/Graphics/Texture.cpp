#include "Texture.h"

Texture::Texture(const std::string &file) {
	set_texture(file);
}

void Texture::compile() {
	load_texture(file, metadata, scratch_image);
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
		srv = GraphicsPipeline::RootSignature::ShaderResourceView(metadata, scratch_image);
	}
}

void Texture::load_texture(const std::string &file, DirectX::TexMetadata &metadata, DirectX::ScratchImage &scratch_image) {
	if (fs::exists(file)) {
		std::string type = to_lower(file.substr(file.find('.')+1));
		if (type == "png" || type == "jpg" || type == "bmp" || type == "gif" || type == "tiff" || type == "jpeg") {
			DirectX::LoadFromWICFile(
				string_to_wstring(file).c_str(),
				WIC_FLAGS_FORCE_SRGB,
				&metadata,
				scratch_image
			);
		} else {
			throw std::exception("Unsupported file type when loading texture.");
		}
	}
}
