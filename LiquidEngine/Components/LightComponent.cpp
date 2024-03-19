#include "LightComponent.h"

LightComponent::LightComponent(Type type) : GraphicsComponent(type) { }

void LightComponent::render_editor_gui_section() {
	FVector4 alb{color_to_fvector(albedo)};
	FVector4 spc{color_to_fvector(specular)};
	float col[4]{alb.x, alb.y, alb.z, alb.w};
	if (ImGui::ColorEdit4("Albedo", col))
		set_albedo(Color{UCHAR(col[0]*255.0f), UCHAR(col[1]*255.0f), UCHAR(col[2]*255.0f), UCHAR(col[3]*255.0f)});

	col[0] = spc.x;
	col[1] = spc.y;
	col[2] = spc.z;
	col[3] = spc.w;
	if (ImGui::ColorEdit4("Specular", col))
		set_specular(Color{UCHAR(col[0]*255.0f), UCHAR(col[1]*255.0f), UCHAR(col[2]*255.0f), UCHAR(col[3]*255.0f)});
	
	if (ImGui::Checkbox("Disabled", &null)) {
		is_null(null);
	}

	render_light_component_editor_gui_section();
}
