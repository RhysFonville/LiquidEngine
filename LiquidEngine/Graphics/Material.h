#pragma once

#include <d3dcompiler.h>
#include <fstream>
#include "../Graphics/Texture.h"
#include "../Components/GraphicsComponent.h"

/**
 * Material data for rendering. Specifies how light interacts with the object.
 */
class Material : public GraphicsTracker {
public:
	Material() : GraphicsTracker{} { }
	Material(const Material& mat);
	Material(const std::string& file);
	Material(const std::string& vs, const std::string& ps, const std::string& data_file = "");

	void compile();
	
	void clean_up();

	void set_data(const std::string &file);
	void set_data(const Material &material);

	GET bool has_texture() const noexcept;
	GET bool has_normal_map() const noexcept;
	GET bool has_environment_texture() const noexcept;

	bool operator==(const Material &material) const noexcept;

	GET Texture & get_albedo_texture() { return albedo_texture; }
	GET Texture & get_normal_map() { return normal_map; }
	GET Texture & get_environment_texture() { return environment_texture; }

	GET Color get_albedo() const { return albedo; }
	void set_albedo(const Color &albedo) { this->albedo = albedo; changed = true; }
	
	GET Color get_specular() const { return specular; }
	void set_specular(const Color &specular) { this->specular = specular; changed = true; }

	GET Color get_ambient() const { return ambient; }
	void set_ambient(const Color &ambient) { this->ambient = ambient; changed = true; }

	GET float get_shininess() const { return shininess; }
	void set_shininess(float shininess) { this->shininess = shininess; changed = true; }

	GraphicsPipeline pipeline{};

	void add_shader_argument(GraphicsPipeline::RootSignature::ConstantBuffer* cb);
	void add_shader_argument(GraphicsPipeline::RootSignature::RootConstants* rc);
	void add_shader_argument(GraphicsPipeline::RootSignature::ShaderResourceView* srv);

	void bind_shader_arguments();

	std::vector<GraphicsPipeline::RootSignature::ConstantBuffer*> get_shader_cbs() const noexcept;
	std::vector<GraphicsPipeline::RootSignature::RootConstants*> get_shader_rcs() const noexcept;
	std::vector<GraphicsPipeline::RootSignature::ShaderResourceView*> get_shader_srvs() const noexcept;

private:
	std::string vs{"Graphics/DefaultVertex.hlsl"};
	std::string hs{};
	std::string ds{};
	std::string gs{};
	std::string ps{"Graphics/LitPixel.hlsl"};

	Texture albedo_texture{};
	Texture normal_map{};
	Texture environment_texture{};
	Color albedo{100, 100, 100, 255}; // kd
	Color specular{5, 5, 5, 255}; // ks
	Color ambient{0, 0, 0, 255}; // ka
	float shininess{10.0f}; // a

	std::vector<std::pair<char, size_t>> order{};
	std::vector<GraphicsPipeline::RootSignature::ConstantBuffer*> cbs{};
	std::vector<GraphicsPipeline::RootSignature::RootConstants*> rcs{};
	std::vector<GraphicsPipeline::RootSignature::ShaderResourceView*> srvs{};
};
