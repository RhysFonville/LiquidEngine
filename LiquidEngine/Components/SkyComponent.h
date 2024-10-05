#pragma once

#include "../Graphics/GraphicsPipeline.h"
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

	bool has_texture() const noexcept;

	GET Texture & get_albedo_texture() { return albedo_texture; }

	GET Color get_albedo() const { return albedo; }
	void set_albedo(const Color &new_albedo) { albedo = new_albedo; changed = true; }

	static Mesh create_sphere(UINT lat_lines, UINT long_lines);

	static const Type component_type = Type::SkyComponent;

	GraphicsPipeline pipeline;

private:
	void render_editor_gui_section() override;

	Texture albedo_texture = Texture();
	Color albedo = Color(0, 0, 0, 255);
	
	std::shared_ptr<GraphicsPipelineMeshChange::Manager> proxy = std::make_shared<GraphicsPipelineMeshChange::Manager>();
};
