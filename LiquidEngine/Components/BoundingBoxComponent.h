#pragma once

#include "PhysicalComponent.h"
#include "../Utility/SimpleShapes.h"

class BoundingBoxComponent : public PhysicalComponent {
public:
	BoundingBoxComponent() : PhysicalComponent{} { }

	SimpleBoundingBox box{};

private:
	void render_physical_component_editor_gui_section() override;
};

