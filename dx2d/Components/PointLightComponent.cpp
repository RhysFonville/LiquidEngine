#include "PointLightComponent.h"

PointLightComponent::PointLightComponent()
	: Component(Component::Type::PointLightComponent), data({ }) { }

bool PointLightComponent::operator==(const PointLightComponent &component) const noexcept {
	return ((Component*)this == (Component*)&component &&
		data.attenuation == component.data.attenuation &&
		data.range == component.data.range &&
		data.diffuse == component.data.diffuse);
}

void PointLightComponent::operator=(const PointLightComponent &light) noexcept {
	data = light.data;
}
