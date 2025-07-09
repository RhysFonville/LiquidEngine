#pragma once

#include "Component.h"
#include "../Physics/PhysicsBody.h"

/**
* Base component for any component that has physics capabilities.
*/
class PhysicalComponent : public Component {
public:
	PhysicalComponent() : Component{} { }

	void tick(float dt) override;

	PhysicsBody physics_body;

protected:
	void render_editor_gui_section() override;
	virtual void render_physical_component_editor_gui_section() { }
};
