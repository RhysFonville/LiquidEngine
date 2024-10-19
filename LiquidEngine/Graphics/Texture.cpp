#include "Texture.h"

Texture::Texture(const std::string &file) {
	set_texture(file);
}

void Texture::compile() {
	set_texture(file);
}

//void Texture::operator==(const Texture &texture) noexcept {
//	image = texture.image;
//	mip_chain = texture.mip_chain;
//	srv.update_signal = true;
//}

bool Texture::operator==(const Texture &texture) const noexcept {
	return (file == texture.file);
}

void Texture::set_texture(const std::string &file, bool enable_mipmap) {
	this->file = file;
	if (exists()) {
		load_texture(file, image, mip_chain, enable_mipmap);
	} else {
		image.Release();
		mip_chain.Release();
	}
	changed = true;
}

void Texture::load_texture(const std::string &file, DirectX::ScratchImage &image, DirectX::ScratchImage &mip_chain, bool enable_mipmap) {
	if (fs::exists(file)) {
		//std::string type = to_lower(file.substr(file.find('.')+1));
		//if (type == "png" || type == "jpg" || type == "jpeg" || type == "bmp" || type == "gif" || type == "tiff" || type == "jpeg") {
			HPEW(DirectX::LoadFromWICFile(
				string_to_wstring(file).c_str(),
				DirectX::WIC_FLAGS_NONE,
				nullptr,
				image
			));
			if (enable_mipmap) {
				DirectX::GenerateMipMaps(
					*image.GetImages(),
					DirectX::TEX_FILTER_BOX,
					0,
					mip_chain
				);
			}
			if (mip_chain.GetImageCount() <= 1 || !enable_mipmap) {
				HPEW(DirectX::LoadFromWICFile(
					string_to_wstring(file).c_str(),
					DirectX::WIC_FLAGS_NONE,
					nullptr,
					mip_chain
				));
			}
		//} else {
		//	throw std::exception("Unsupported file type when loading texture.");
		//}
	}
}

bool Texture::is_opaque() const noexcept {
	return image.IsAlphaAllOpaque();
}
