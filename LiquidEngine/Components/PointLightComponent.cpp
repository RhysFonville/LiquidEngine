#include "PointLightComponent.h"

PointLightComponent::PointLightComponent()
	: LightComponent(Component::Type::PointLightComponent) { }

bool PointLightComponent::operator==(const PointLightComponent &component) const noexcept {
	return ((Component*)this == (Component*)&component &&
		attenuation == component.attenuation &&
		range == component.range &&
		albedo == component.albedo);
}

void PointLightComponent::operator=(const PointLightComponent &light) noexcept {
	range = light.range;
	attenuation = light.attenuation;
	albedo = light.albedo;
	specular = light.specular;
	null = light.null;
}
