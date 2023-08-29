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
