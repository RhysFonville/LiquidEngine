#include "SpotlightComponent.h"

SpotlightComponent::SpotlightComponent()
	: LightComponent(Type::SpotlightComponent) { }

bool SpotlightComponent::operator==(const SpotlightComponent &component) const noexcept {
	return ((Component*)this == (Component*)&component &&
		direction == component.direction &&
		albedo == component.albedo &&
		specular == component.specular);
}

void SpotlightComponent::operator=(const SpotlightComponent &component) noexcept {
	direction = component.direction;
	albedo = component.albedo;
	specular = component.specular;
}
