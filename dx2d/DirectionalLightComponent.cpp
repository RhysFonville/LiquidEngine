#include "DirectionalLightComponent.h"

DirectionalLightComponent::DirectionalLightComponent()
	: Component(Type::DirectionalLightComponent), direction(FVector3(0.25f, 0.5f, -1.0f)),
	diffuse(255.0f, 255.0f, 255.0f, 255.0f), specular(0.5f, 0.5f, 0.5f, 1.0f) { }

bool DirectionalLightComponent::operator==(const DirectionalLightComponent &component) const noexcept {
	return ((Component*)this == (Component*)&component &&
			direction == component.direction &&
			diffuse == component.diffuse);
}

void DirectionalLightComponent::operator=(const DirectionalLightComponent &component) noexcept {
	direction = component.direction;
	diffuse = component.diffuse;
	specular = component.specular;
}
