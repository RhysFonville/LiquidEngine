#include "Texture.h"

Texture::Texture() { }

Texture::Texture(std::string file_name) {
	this->file_name = file_name;
}

void Texture::clean_up() {
	COMPTR_RELEASE(texture);
	COMPTR_RELEASE(texture_view);
	//COMPTR_RELEASE(sampler_state);
}

bool Texture::operator==(const Texture &texture) const noexcept {
	if (this->texture.Get() == texture.texture.Get() &&
		texture_view.Get() == texture.texture_view.Get() &&
		sampler_state.Get() == texture.sampler_state.Get() &&
		file_name == texture.file_name)
		return true;
	else
		return false;
}
