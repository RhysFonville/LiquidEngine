#include "PointLightComponent.h"

PointLightComponent::PointLightComponent()
	: LightComponent(Component::Type::PointLightComponent) { }

bool PointLightComponent::operator==(const PointLightComponent &component) const noexcept {
	return ((Component*)this == (Component*)&component &&
		range == component.range &&
		attenuation == component.attenuation &&
		albedo == component.albedo);
}

void PointLightComponent::operator=(const PointLightComponent &light) noexcept {
	range = light.range;
	attenuation = light.attenuation;
	albedo = light.albedo;
	specular = light.specular;
	null = light.null;
}

void PointLightComponent::render_light_component_editor_gui_section() {
	float range{this->range};
	if (ImGui::InputFloat("Range", &range)) {
		set_range(range);
	}

	float xs[1000], ys[1000];
	for (int i = 0; i < 1000; i++) {
		xs[i] = i * 0.001f;
		ys[i] = attenuation.x + attenuation.y*xs[i] + attenuation.z*(xs[i]*xs[i]);
	}
	if (ImPlot::BeginPlot("Line Plots")) {
		ImPlot::SetupAxes("x","y");
		ImPlot::PlotLine("f(x)", xs, ys, 1000);
		ImPlot::EndPlot();
	}
	float att[3]{attenuation.x, attenuation.y, attenuation.z};
	if (ImGui::InputFloat3("Attenuation", att)) {
		set_attenuation(att);
	}
}
