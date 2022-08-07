#include "Material.h"

Material::Material(std::string name) : name(name) {
	compile();
}

void Material::compile(bool compile_texture) {
	// Create Pixel Shader
	HANDLE_POSSIBLE_EXCEPTION(D3DReadFileToBlob(string_to_wstring(pixel_shader_name).c_str(), pixel_blob.GetAddressOf()));

	// Create Vertex Shader
	HANDLE_POSSIBLE_EXCEPTION(D3DReadFileToBlob(string_to_wstring(vertex_shader_name).c_str(), vertex_blob.GetAddressOf()));
}

void Material::clean_up() {
	texture.clean_up();

	COMPTR_RELEASE(input_layout);
	COMPTR_RELEASE(vertex_shader);
	COMPTR_RELEASE(pixel_shader);
	COMPTR_RELEASE(vertex_blob);
	COMPTR_RELEASE(pixel_blob);
}

void Material::read_mtl_file(std::vector<std::string> contents) noexcept {
	for (std::string line : contents) {
		if (line.substr(0, 6) == "map_Kd") {
			texture = Texture(line.substr(7));
		}
		if (line.substr(0, 8) == "map_Bump") {
			normal_map = Texture(line.substr(9));
		}
		if (line.substr(0, 2) == "Kd") {
			std::vector<std::string> colors = split(line, ' ');

			diffuse = Color({ (UCHAR)(std::stof(colors[1]) * 255.0f),
							  (UCHAR)(std::stof(colors[2]) * 255.0f),
							  (UCHAR)(std::stof(colors[3]) * 255.0f),
							  (UCHAR)255 });
		}
	}
}

bool Material::operator==(const Material &material) const noexcept {
	if (texture == material.texture &&
		input_layout.Get() == material.input_layout.Get() &&
		vertex_shader.Get() == material.vertex_shader.Get() &&
		pixel_shader.Get() == material.pixel_shader.Get() &&
		vertex_blob.Get() == material.vertex_blob.Get() &&
		pixel_blob.Get() == material.pixel_blob.Get() &&
		pixel_shader_name == material.pixel_shader_name &&
		vertex_shader_name == material.vertex_shader_name)
		return true;
	else
		return false;
}

Material::operator ConstantBufferStruct() const noexcept {
	ConstantBufferStruct cbs;
	cbs.has_texture = texture == Texture() ? false : true;
	cbs.has_normal_map = normal_map == Texture() ? false : true;
	
	cbs.diffuse = XMFLOAT4(diffuse.r / 255.0f, diffuse.g / 255.0f, diffuse.b / 255.0f, diffuse.a / 255.0f);
	cbs.specular = specular;
	cbs.shininess = shininess;

	return cbs;
}
