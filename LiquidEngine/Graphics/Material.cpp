#include "Material.h"

Material::Material(const Material &mat) : Material{} {
	set_data(mat);
}

Material::Material(const std::string &file) : Material{} {
	set_data(file);
}

void Material::compile() {
	pipeline.vs = vs;
	pipeline.hs = hs;
	pipeline.ds = ds;
	pipeline.gs = gs;
	pipeline.ps = ps;

	if (has_texture()) {
		albedo_texture.compile();
	}
	if (has_normal_map()) {
		normal_map.compile();
	}

	pipeline.compilation_signal = true;
}

void Material::clean_up() {
	pipeline.clean_up();
}

void Material::set_data(const std::string &file) {
	std::ifstream read(file);
	if (read.is_open()) {
		std::string line;
		while (std::getline(read, line)) {
			if (line.substr(0, 2) == "Ns") {
				shininess = std::stof(line.substr(3)) * 0.0025f;
			}
			if (line.substr(0, 2) == "Ka") {
				std::vector<std::string> colors = split(line, ' ');

				ambient = Color({
					(UCHAR)(std::stof(colors[1]) * 255.0f),
					(UCHAR)(std::stof(colors[2]) * 255.0f),
					(UCHAR)(std::stof(colors[3]) * 255.0f),
					(UCHAR)255
				});
				ambient = Color{0, 0, 0, 255};
			}
			if (line.substr(0, 2) == "Kd") {
				std::vector<std::string> colors = split(line, ' ');

				albedo = Color({
					(UCHAR)(std::stof(colors[1]) * 255.0f),
					(UCHAR)(std::stof(colors[2]) * 255.0f),
					(UCHAR)(std::stof(colors[3]) * 255.0f),
					(UCHAR)255
				});
			}
			if (line.substr(0, 2) == "Ks") {
				std::vector<std::string> colors = split(line, ' ');

				specular = Color({
					(UCHAR)(std::stof(colors[1]) * 255.0f),
					(UCHAR)(std::stof(colors[2]) * 255.0f),
					(UCHAR)(std::stof(colors[3]) * 255.0f),
					(UCHAR)255
				});
			}
			if (line.substr(0, 2) == "d ") {
				albedo.a = (UCHAR)(std::stof(line.substr(2)) * 255.0f);
			}

			if (line.substr(0, 6) == "map_Kd") {
				albedo_texture.set_texture(line.substr(7));
			}
			if (line.substr(0, 8) == "map_Bump") {
				normal_map.set_texture(line.substr(9));
			}
		}
		albedo /= 2;
		specular /= 8;

		changed = true;
	} else {
		throw std::exception("Material set_data file could not be opened.");
	}
}

void Material::set_data(const Material &material) {
	albedo_texture.set_texture(material.albedo_texture.get_file());
	normal_map.set_texture(material.normal_map.get_file());
	specular = material.specular;
	albedo = material.albedo;
	ambient = material.ambient;
	shininess = material.shininess;

	changed = true;
}

bool Material::has_texture() const noexcept {
	return albedo_texture.exists();
}

bool Material::has_normal_map() const noexcept {
	return normal_map.exists();
}

bool Material::has_environment_texture() const noexcept {
	return environment_texture.exists();
}

void Material::operator=(const Material &material) noexcept {
	set_data(material);
}

bool Material::operator==(const Material &material) const noexcept {
	return (
		vs == material.vs &&
		hs == material.hs &&
		ds == material.ds &&
		gs == material.gs &&
		ps == material.ps &&
		
		specular == material.specular &&
		albedo == material.albedo &&
		ambient == material.ambient &&
		shininess == material.shininess);
}
