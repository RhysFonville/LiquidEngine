#pragma once

#include <d3dcompiler.h>
#include <fstream>
#include "../Graphics/Texture.h"
#include "../Components/GraphicsComponent.h"

/**
 * Material data for rendering. Specifies how light interacts with the object.
 */
class MaterialComponent : public GraphicsComponent {
public:
	MaterialComponent() : GraphicsComponent{Component::Type::MaterialComponent} { }
	MaterialComponent(const MaterialComponent &mat);
	MaterialComponent(const std::string &file);

	void compile() override;
	
	void clean_up() override;

	void set_data(const std::string &file);
	void set_data(const MaterialComponent &material);

	bool has_texture() const noexcept;
	bool has_normal_map() const noexcept;

	void operator=(const MaterialComponent &material) noexcept;
	bool operator==(const MaterialComponent &material) const noexcept;

	Texture & get_albedo_texture() { return albedo_texture; }
	Texture & get_normal_map() { return normal_map; }

	Color get_albedo() const { return albedo; }
	void set_albedo(const Color &albedo) { this->albedo = albedo; changed = true; }
	
	Color get_specular() const { return specular; }
	void set_specular(const Color &specular) { this->specular = specular; changed = true; }

	Color get_ambient() const { return ambient; }
	void set_ambient(const Color &ambient) { this->ambient = ambient; changed = true; }

	float get_shininess() const { return shininess; }
	void set_shininess(const float &shininess) { this->shininess = shininess; changed = true; }

	GraphicsPipeline pipeline;

	static const Type component_type = Type::MaterialComponent;

private:
	std::string vs{"Graphics/DefaultVertex.hlsl"};
	std::string hs{};
	std::string ds{};
	std::string gs{};
	std::string ps{"Graphics/LitPixel.hlsl"};

	Texture albedo_texture{};
	Texture normal_map{};
	Color albedo{100, 100, 100, 255}; // kd
	Color specular{5, 5, 5, 255}; // ks
	Color ambient{0, 0, 0, 255}; // ka
	float shininess{0.5f}; // a
};
