#include "SpotlightComponent.h"

SpotlightComponent::SpotlightComponent()
	: Component(Type::SpotlightComponent), direction(0, 0, 0), ambient(0, 0, 0, 0),
	diffuse(0, 0, 0, 0) { }

void SpotlightComponent::operator=(const SpotlightComponent &light) noexcept {
	transform = light.transform;
	direction = light.direction;
	ambient = light.ambient;
	diffuse = light.diffuse;
}

