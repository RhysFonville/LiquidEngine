#pragma once

#include "PhysicalComponent.h"
#include "../Utility/SimpleShapes.h"

class BoundingBoxComponent : public PhysicalComponent {
public:
	BoundingBoxComponent() : PhysicalComponent{Type::BoundingBoxComponent} { }

	SimpleBoundingBox box{};

	static const Type component_type = Type::BoundingBoxComponent;

private:
	void render_physical_component_editor_gui_section() override;
};

