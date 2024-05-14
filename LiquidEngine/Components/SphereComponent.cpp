#include "SphereComponent.h"

void SphereComponent::render_physical_component_editor_gui_section() {
	float radius{this->sphere.radius};
	if (ImGui::InputFloat("Radius", &radius)) {
		sphere.radius = radius;
	}
}

