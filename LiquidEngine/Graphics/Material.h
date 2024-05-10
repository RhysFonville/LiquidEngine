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
	Material(const Material &mat);
	Material(const std::string &file);

	void compile();
	
	void clean_up();

	void set_data(const std::string &file);
	void set_data(const Material &material);

	GET bool has_texture() const noexcept;
	GET bool has_normal_map() const noexcept;
	GET bool has_environment_texture() const noexcept;

	void operator=(const Material &material) noexcept;
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
};
