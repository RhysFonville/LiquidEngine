#include "Texture.h"

Texture::Texture(const std::string &file) {
	set_texture(file);
}

void Texture::compile() {
	set_texture(file);
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
	if (file != this->file) {
		this->file = file;
		if (exists()) {
			load_texture(file, image, mip_chain);
			srv = GraphicsPipeline::RootSignature::ShaderResourceView(mip_chain);
		}
	}
}

void Texture::load_texture(const std::string &file, DirectX::ScratchImage &image, DirectX::ScratchImage &mip_chain) {
	if (fs::exists(file)) {
		std::string type = to_lower(file.substr(file.find('.')+1));
		if (type == "png" || type == "jpg" || type == "jpeg" || type == "bmp" || type == "gif" || type == "tiff" || type == "jpeg") {
			HPEW(DirectX::LoadFromWICFile(
				string_to_wstring(file).c_str(),
				DirectX::WIC_FLAGS_NONE,
				nullptr,
				image
			));
			DirectX::GenerateMipMaps(
				*image.GetImages(),
				DirectX::TEX_FILTER_BOX,
				0,
				mip_chain
			);
			if (mip_chain.GetImageCount() <= 1) {
				HPEW(DirectX::LoadFromWICFile(
					string_to_wstring(file).c_str(),
					DirectX::WIC_FLAGS_NONE,
					nullptr,
					mip_chain
				));
			}
		} else {
			throw std::exception("Unsupported file type when loading texture.");
		}
	}
}
