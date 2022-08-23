#include "PointLightComponent.h"

PointLightComponent::PointLightComponent()
	: Component(Component::Type::PointLightComponent), attenuation(FVector3(0.0f, 0.2f, 0.0f)),
	range(100.0f), diffuse(FVector4(1.0f, 1.0f, 1.0f, 1.0f)) { }

void PointLightComponent::operator=(const PointLightComponent &light) noexcept {
	transform = light.transform;
	attenuation = light.attenuation;
	range = light.range;
	diffuse = light.diffuse;
}
