#include "Material.h"

void Material::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const UVector2 &resolution) {
	compile();
	pipeline.compile(device, command_list, sample_desc, resolution);

	pipeline.compilation_signal = false;
}

void Material::compile() {
	pipeline.vs = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Vertex, vs);
	pipeline.hs = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Hull, hs);
	pipeline.ds = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Domain, ds);
	pipeline.gs = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Geometry, gs);
	pipeline.ps = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Pixel, ps);

	if (has_texture()) {
		diffuse_texture.compile();
		pipeline.root_signature.bind_shader_resource_view(
			diffuse_texture.srv,
			D3D12_SHADER_VISIBILITY_PIXEL
		);
	}

	if (has_normal_map()) {
		normal_map.compile();
		pipeline.root_signature.bind_shader_resource_view(
			normal_map.srv,
			D3D12_SHADER_VISIBILITY_PIXEL
		);
	}

	pipeline.compilation_signal = true;
}

void Material::clean_up() { }

void Material::set_data(const std::string &file) {
	std::ifstream read(file);
	if (read.is_open()) {
		std::string line;
		while (std::getline(read, line)) {
			if (line.substr(0, 2) == "Ns") {
				shininess = std::stof(line.substr(3)) / 900.0f;
			}
			if (line.substr(0, 2) == "Ka") {
				std::vector<std::string> colors = split(line, ' ');

				ambient = Color({
					(UCHAR)(std::stof(colors[1]) * 255.0f),
					(UCHAR)(std::stof(colors[2]) * 255.0f),
					(UCHAR)(std::stof(colors[3]) * 255.0f),
					(UCHAR)255
				});
			}
			if (line.substr(0, 2) == "Kd") {
				std::vector<std::string> colors = split(line, ' ');

				diffuse = Color({
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
				diffuse.a = (UCHAR)(std::stof(line.substr(2)) * 255.0f);
			}

			/*if (line.substr(0, 6) == "map_Kd") {
				texture = Texture(line.substr(7));
			}
			if (line.substr(0, 8) == "map_Bump") {
				normal_map = Texture(line.substr(9));
			}*/
		}
		shininess *= 13;
		specular /= 6;
	} else {
		throw std::exception("Material set_data file could not be opened.");
	}
}

bool Material::has_texture() const noexcept {
	return texture_exists(diffuse_texture);
}

bool Material::has_normal_map() const noexcept {
	return texture_exists(normal_map);
}

void Material::operator=(const Material &material) noexcept {
	vs = material.vs;
	hs = material.hs;
	ds = material.ds;
	gs = material.gs;
	ps = material.ps;

	//data = material.data;
}

bool Material::operator==(const Material &material) const noexcept {
	return (vs == material.vs &&
		hs == material.hs &&
		ds == material.ds &&
		gs == material.gs &&
		ps == material.ps &&
		
		specular == material.specular &&
		diffuse == material.diffuse &&
		ambient == material.ambient &&
		shininess == material.shininess);
}
