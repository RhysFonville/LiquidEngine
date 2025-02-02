#pragma once

#include <d3dcompiler.h>
#include <fstream>
#include "Pipeline/GraphicsPipeline.h"
#include "Texture.h"
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
	GET bool has_specular_map() const noexcept;

	bool operator==(const Material &material) const noexcept;

	GET Texture & get_albedo_texture() noexcept { return albedo_texture; }
	GET Texture & get_normal_map() noexcept { return normal_map; }
	GET Texture & get_specular_map() noexcept { return specular_map; }

	GET Color get_albedo() const noexcept { return albedo; }
	void set_albedo(const Color &albedo) noexcept { this->albedo = albedo; changed = true; }
	
	GET Color get_specular() const noexcept { return specular; }
	void set_specular(const Color &specular) noexcept { this->specular = specular; changed = true; }

	GET Color get_ambient() const noexcept { return ambient; }
	void set_ambient(const Color &ambient) noexcept  { this->ambient = ambient; changed = true; }

	GET float get_shininess() const noexcept { return shininess; }
	void set_shininess(float shininess) noexcept { this->shininess = shininess; changed = true; }

	GET bool is_opaque() const noexcept;

	GraphicsPipeline pipeline{};

private:
	std::string vs{"Graphics/Shaders/DefaultVertex.hlsl"};
	std::string hs{};
	std::string ds{};
	std::string gs{};
	std::string ps{"Graphics/Shaders/LitPixel.hlsl"};

	Texture albedo_texture{};
	Texture normal_map{};
	Texture specular_map{};
	Color albedo{255, 255, 255, 255}; // kd
	Color specular{5, 5, 5, 255}; // ks
	Color ambient{0, 0, 0, 255}; // ka
	float shininess{10.0f}; // a
};
