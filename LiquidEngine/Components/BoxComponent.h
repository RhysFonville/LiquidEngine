#pragma once

#include "PhysicalComponent.h"

class BoxComponent : public PhysicalComponent {
public:
	BoxComponent() : PhysicalComponent(Type::BoxComponent) { }
};
