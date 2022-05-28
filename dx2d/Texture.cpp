#include "Texture.h"

Texture::Texture() {
	file_name = std::filesystem::current_path().u8string() + "\\" + "test_image.jpg";
}

Texture::Texture(std::string file_name) {
	this->file_name = file_name;
}
