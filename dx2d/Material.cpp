#include "Material.h"

Material::Material() {
	compile();
}

void Material::compile() {
	//// Create Pixel Shader
	//HANDLE_POSSIBLE_EXCEPTION_WINDOWS(D3DReadFileToBlob(string_to_wstring(pixel_shader).c_str(), pixel_blob.GetAddressOf()));

	//// Create Vertex Shader
	//HANDLE_POSSIBLE_EXCEPTION_WINDOWS(D3DReadFileToBlob(string_to_wstring(vertex_shader).c_str(), vertex_blob.GetAddressOf()));
}

void LitMaterial::read_mtl_file(std::vector<std::string> contents) noexcept {
	for (std::string line : contents) {
		if (line.substr(0, 2) == "Ns") {
			a = std::stof(line.substr(3)) / 900.0f;
		}
		if (line.substr(0, 2) == "Ka") {
			std::vector<std::string> colors = split(line, ' ');

			ka = Color({
						(UCHAR)(std::stof(colors[1]) * 255.0f),
						(UCHAR)(std::stof(colors[2]) * 255.0f),
						(UCHAR)(std::stof(colors[3]) * 255.0f),
						(UCHAR)255
				});
		}
		if (line.substr(0, 2) == "Kd") {
			std::vector<std::string> colors = split(line, ' ');

			kd = Color({
						(UCHAR)(std::stof(colors[1]) * 255.0f),
						(UCHAR)(std::stof(colors[2]) * 255.0f),
						(UCHAR)(std::stof(colors[3]) * 255.0f),
						(UCHAR)255
				});
		}
		if (line.substr(0, 2) == "Ks") {
			std::vector<std::string> colors = split(line, ' ');

			ks = Color({
				(UCHAR)(std::stof(colors[1]) * 255.0f),
				(UCHAR)(std::stof(colors[2]) * 255.0f),
				(UCHAR)(std::stof(colors[3]) * 255.0f),
				(UCHAR)255
				});
		}
		if (line.substr(0, 2) == "d ") {
			kd.a = (UCHAR)(std::stof(line.substr(2)) * 255.0f);
		}

		if (line.substr(0, 6) == "map_Kd") {
			texture = Texture(line.substr(7));
		}
		if (line.substr(0, 8) == "map_Bump") {
			normal_map = Texture(line.substr(9));
		}
	}
	a *= 13.0f;
	ks /= 6.0f;
}

bool LitMaterial::operator==(const LitMaterial &material) {
	cbs.ks = color_to_XMFLOAT4(ks, true);
	cbs.kd = color_to_XMFLOAT4(kd, true);
	cbs.ka = color_to_XMFLOAT4(ka, true);
	cbs.a = a;

	return cbs;
}
