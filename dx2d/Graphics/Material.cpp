#include "Material.h"

void Material::compile(GraphicsPipeline &pipeline) {
	pipeline.vs = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Vertex, vs);
	pipeline.hs = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Hull, hs);
	pipeline.ds = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Domain, ds);
	pipeline.gs = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Geometry, gs);
	pipeline.ps = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Pixel, ps);
}

void Material::clean_up() { }

void Material::read_mtl_file(std::vector<std::string> contents) noexcept {
	for (std::string line : contents) {
		if (line.substr(0, 2) == "Ns") {
			data.a = std::stof(line.substr(3)) / 900.0f;
		}
		if (line.substr(0, 2) == "Ka") {
			std::vector<std::string> colors = split(line, ' ');

			data.ka = Color({
						(UCHAR)(std::stof(colors[1]) * 255.0f),
						(UCHAR)(std::stof(colors[2]) * 255.0f),
						(UCHAR)(std::stof(colors[3]) * 255.0f),
						(UCHAR)255
				});
		}
		if (line.substr(0, 2) == "Kd") {
			std::vector<std::string> colors = split(line, ' ');

			data.kd = Color({
						(UCHAR)(std::stof(colors[1]) * 255.0f),
						(UCHAR)(std::stof(colors[2]) * 255.0f),
						(UCHAR)(std::stof(colors[3]) * 255.0f),
						(UCHAR)255
				});
		}
		if (line.substr(0, 2) == "Ks") {
			std::vector<std::string> colors = split(line, ' ');

			data.ks = Color({
				(UCHAR)(std::stof(colors[1]) * 255.0f),
				(UCHAR)(std::stof(colors[2]) * 255.0f),
				(UCHAR)(std::stof(colors[3]) * 255.0f),
				(UCHAR)255
				});
		}
		if (line.substr(0, 2) == "d ") {
			data.kd.a = (UCHAR)(std::stof(line.substr(2)) * 255.0f);
		}

		/*if (line.substr(0, 6) == "map_Kd") {
			texture = Texture(line.substr(7));
		}
		if (line.substr(0, 8) == "map_Bump") {
			normal_map = Texture(line.substr(9));
		}*/
	}
	data.a *= 13;
	data.ks /= 6;
}

void Material::operator=(const Material &material) noexcept {
	vs = material.vs;
	hs = material.hs;
	ds = material.ds;
	gs = material.gs;
	ps = material.ps;

	data = material.data;
}

bool Material::operator==(const Material &material) const noexcept {
	return (vs == material.vs &&
		hs == material.hs &&
		ds == material.ds &&
		gs == material.gs &&
		ps == material.ps &&
		
		data == material.data);
}
