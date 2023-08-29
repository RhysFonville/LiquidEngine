#include "DirectionalLightComponent.h"

DirectionalLightComponent::DirectionalLightComponent()
	: LightComponent(Type::DirectionalLightComponent) { }

bool DirectionalLightComponent::operator==(const DirectionalLightComponent &component) const noexcept {
	return ((Component*)this == (Component*)&component &&
		direction == component.direction &&
		albedo == component.albedo);
}

void DirectionalLightComponent::operator=(const DirectionalLightComponent &component) noexcept {
	direction = component.direction;
	albedo = component.albedo;
	specular = component.specular;
	null = component.null;
}
