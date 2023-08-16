#include "Texture.h"

Texture::Texture() { }

Texture::Texture(const std::string &file) {
	set_texture(file);
}

void Texture::compile() {
	DirectX::LoadFromWICFile(
		string_to_wstring(file).c_str(),
		WIC_FLAGS_FORCE_RGB,
		&metadata,
		scratch_image
	);
}

void Texture::clean_up() {
	/*COMPTR_RELEASE(texture);
	COMPTR_RELEASE(texture_view);*/
	//COMPTR_RELEASE(sampler_state);
}

bool Texture::operator==(const Texture &texture) const noexcept {
	if (/*this->texture.Get() == texture.texture.Get() &&
		texture_view.Get() == texture.texture_view.Get() &&
		sampler_state.Get() == texture.sampler_state.Get() &&*/
		file == texture.file)
		return true;
	else
		return false;
}

void Texture::set_texture(const std::string &file) {
	this->file = file;
	if (fs::exists(file)) {
		compile();
		srv = GraphicsPipeline::RootSignature::ShaderResourceView(metadata, scratch_image);
	}
}
