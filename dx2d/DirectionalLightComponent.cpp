#include "DirectionalLightComponent.h"

DirectionalLightComponent::DirectionalLightComponent()
	: Component(Type::DirectionalLightComponent), data({ }) { }

bool DirectionalLightComponent::operator==(const DirectionalLightComponent &component) const noexcept {
	return ((Component*)this == (Component*)&component &&
		data.direction == component.data.direction &&
		data.diffuse == component.data.diffuse);
}

void DirectionalLightComponent::operator=(const DirectionalLightComponent &component) noexcept {
	data = component.data;
}
