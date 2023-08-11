#include "SpotlightComponent.h"

SpotlightComponent::SpotlightComponent()
	: LightComponent(Type::SpotlightComponent) { }

bool SpotlightComponent::operator==(const SpotlightComponent &component) const noexcept {
	return ((Component*)this == (Component*)&component &&
		data.direction == component.data.direction &&
		data.diffuse == component.data.diffuse &&
		data.specular == component.data.specular);
}

void SpotlightComponent::operator=(const SpotlightComponent &component) noexcept {
	data = component.data;
}
