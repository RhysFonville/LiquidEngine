#include "Material.h"

Material::Material(const Material& mat)
	: vs{mat.vs}, hs{mat.hs}, ds{mat.ds}, gs{mat.gs}, ps{mat.ps}, GraphicsTracker{} {
	set_data(mat);
}

Material::Material(const std::string& file) : GraphicsTracker{} {
	set_data(file);
}

Material::Material(const std::string& vs, const std::string& ps, const std::string& data_file)
	: GraphicsTracker{}, vs{vs}, ps{ps} {
	if (!data_file.empty()) set_data(data_file);
}

void Material::compile() {
	pipeline.vs = shader_storage->add_and_compile_shader(Shader::Type::Vertex, vs);
	pipeline.hs = shader_storage->add_and_compile_shader(Shader::Type::Hull, hs);
	pipeline.ds = shader_storage->add_and_compile_shader(Shader::Type::Domain, ds);
	pipeline.gs = shader_storage->add_and_compile_shader(Shader::Type::Geometry, gs);
	pipeline.ps = shader_storage->add_and_compile_shader(Shader::Type::Pixel, ps);

	if (has_texture()) {
		albedo_texture.compile();
	}
	if (has_normal_map()) {
		normal_map.compile();
	}

	pipeline.compile();
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

bool Material::has_specular_map() const noexcept {
	return specular_map.exists();
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
		shininess == material.shininess
	);
}

bool Material::is_opaque() const noexcept {
	bool is_albedo_opaque = albedo_texture.exists() ? albedo_texture.is_opaque() : (albedo.a == 1.0f);
	bool is_normal_opaque = normal_map.exists() ? normal_map.is_opaque() : (albedo.a == 1.0f);
	bool is_specular_opaque = specular_map.exists() ? specular_map.is_opaque() : (albedo.a == 1.0f);
	bool is_ambient_opaque = (ambient.a == 1.0f);

	return (is_albedo_opaque && is_normal_opaque && is_specular_opaque && is_ambient_opaque);
}

void Material::add_shader_argument(std::shared_ptr<ConstantBuffer>& cb) {
	order.push_back(std::make_pair(0, cbs.size()));
	cbs.push_back(cb);
}

void Material::add_shader_argument(std::shared_ptr<RootConstants>& rc) {
	order.push_back(std::make_pair(1, rcs.size()));
	rcs.push_back(rc);
}

void Material::add_shader_argument(std::shared_ptr<ShaderResourceView>& srv) {
	order.push_back(std::make_pair(2, srvs.size()));
	srvs.push_back(srv);
}

void Material::bind_shader_arguments() {
	for (auto argument : order) {
		if (argument.first == 0) {
			pipeline.root_signature.bind_constant_buffer(cbs[argument.second], D3D12_SHADER_VISIBILITY_PIXEL);
		} else if (argument.first == 1) {
			pipeline.root_signature.bind_root_constants(rcs[argument.second], D3D12_SHADER_VISIBILITY_PIXEL);
		} else if (argument.first == 2) {
			pipeline.root_signature.bind_shader_resource_view(srvs[argument.second], D3D12_SHADER_VISIBILITY_PIXEL);
		}
	}
}

std::vector<std::shared_ptr<ConstantBuffer>> Material::get_shader_cbs() const noexcept {
	return cbs;
}

std::vector<std::shared_ptr<RootConstants>> Material::get_shader_rcs() const noexcept {
	return rcs;
}

std::vector<std::shared_ptr<ShaderResourceView>> Material::get_shader_srvs() const noexcept {
	return srvs;
}
