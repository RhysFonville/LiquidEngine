#include "SpotlightComponent.h"

SpotlightComponent::SpotlightComponent()
	: Component(Type::SpotlightComponent), direction(0, 0, 0),
	diffuse(0, 0, 0, 0) { }

bool SpotlightComponent::operator==(const SpotlightComponent &component) const noexcept {
	return ((Component*)this == (Component*)&component &&
			direction == component.direction &&
			diffuse == component.diffuse &&
			specular == component.specular);
}

void SpotlightComponent::operator=(const SpotlightComponent &component) noexcept {
	direction = component.direction;
	diffuse = component.diffuse;
	specular = component.specular;
}
