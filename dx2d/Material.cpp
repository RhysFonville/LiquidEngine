#include "Material.h"

void Material::compile(GraphicsPipeline &pipeline) {
	pipeline.vs = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Vertex, vs);
	pipeline.hs = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Hull, hs);
	pipeline.ds = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Domain, ds);
	pipeline.gs = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Geometry, gs);
	pipeline.ps = GraphicsPipeline::Shader(GraphicsPipeline::Shader::Type::Pixel, ps);

	//cb = GraphicsPipeline::RootSignature::ConstantBuffer(PerObjectPSCB({ cbs }));
	//pipeline.root_signature.add_constant_buffer(cb, D3D12_SHADER_VISIBILITY_PIXEL);
}

void Material::clean_up() { }

void Material::read_mtl_file(std::vector<std::string> contents) noexcept {
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

		/*if (line.substr(0, 6) == "map_Kd") {
			texture = Texture(line.substr(7));
		}
		if (line.substr(0, 8) == "map_Bump") {
			normal_map = Texture(line.substr(9));
		}*/
	}
	a *= 13.0f;
	ks /= 6.0f;
}

void Material::operator=(const Material &material) noexcept {
	vs = material.vs;
	hs = material.hs;
	ds = material.ds;
	gs = material.gs;
	ps = material.ps;

	ks = material.ks;
	kd = material.kd;
	ka = material.ka;
	a = material.a;

	cbs = material.cbs;
	cb = material.cb;
}

bool Material::operator==(const Material &material) const noexcept {
	return (vs == material.vs &&
		hs == material.hs &&
		ds == material.ds &&
		gs == material.gs &&
		ps == material.ps &&
		
		ks == material.ks &&
		kd == material.kd &&
		ka == material.ka &&
		a == material.a &&

		cb == material.cb);
}
