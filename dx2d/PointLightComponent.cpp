#include "PointLightComponent.h"

PointLightComponent::PointLightComponent()
	: Component(Component::Type::PointLightComponent), attenuation(FVector3(0.0f, 0.2f, 0.0f)),
	range(100.0f), diffuse(FVector4(1.0f, 1.0f, 1.0f, 1.0f)) { }

bool PointLightComponent::operator==(const PointLightComponent &component) const noexcept {
	return ((Component*)this == (Component*)&component &&
		attenuation == component.attenuation &&
		range == component.range &&
		diffuse == component.diffuse);
}

void PointLightComponent::operator=(const PointLightComponent &light) noexcept {
	attenuation = light.attenuation;
	range = light.range;
	diffuse = light.diffuse;
}
