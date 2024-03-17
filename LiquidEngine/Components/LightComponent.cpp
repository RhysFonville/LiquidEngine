#include "LightComponent.h"

LightComponent::LightComponent(Type type) : GraphicsComponent(type) { }

void LightComponent::render_editor_gui_section() {
	float col[4]{albedo.r, albedo.g, albedo.b, albedo.a};
	if (ImGui::ColorEdit4("Albedo", col))
		set_albedo(Color{(UCHAR)col[0], (UCHAR)col[1], (UCHAR)col[2], (UCHAR)col[3]});

	col[0] = specular.r;
	col[1] = specular.g;
	col[2] = specular.b;
	col[3] = specular.a;
	if (ImGui::ColorEdit3("Specular", col))
		set_specular(Color{(UCHAR)col[0], (UCHAR)col[1], (UCHAR)col[2], (UCHAR)col[3]});
	
	if (ImGui::Checkbox("Disabled", &null)) {
		is_null(null);
	}

	render_light_component_editor_gui_section();
}
