#pragma once

#include "../Graphics/Pipeline/GraphicsPipeline.h"
#include "../Graphics/Texture.h"
#include "GraphicsComponent.h"
#include "../imgui/imgui_stdlib.h"

/**
* Component for a sky.
*/
class SkyComponent : public GraphicsComponent {
public:
	SkyComponent();

	void compile() override;

	void clean_up() override;

	bool has_texture() const noexcept;

	GET Texture & get_albedo_texture() { return albedo_texture; }

	GET Color get_albedo() const { return albedo; }
	void set_albedo(const Color &new_albedo) { albedo = new_albedo; changed = true; }

	static Mesh create_sphere(UINT lat_lines, UINT long_lines) { }

	GraphicsPipeline pipeline;

private:
	void render_editor_gui_section() override;

	Texture albedo_texture = Texture();
	Color albedo = Color(0, 0, 0, 255);
};
