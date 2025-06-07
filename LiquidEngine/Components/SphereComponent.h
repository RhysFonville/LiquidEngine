#pragma once

#include "PhysicalComponent.h"

/**
* Component for a box used for physics.
*/
class SphereComponent : public PhysicalComponent {
public:
	SphereComponent() : PhysicalComponent{} { }

	Sphere sphere{};

private:
	void render_physical_component_editor_gui_section() override;
};
