#include "DirectionalLightComponent.h"

DirectionalLightComponent::DirectionalLightComponent()
	: Component(Type::DirectionalLightComponent), direction(FVector3(0.25f, 0.5f, -1.0f)),
	ambient(0.0f, 0.0f, 0.0f, 0.0f), diffuse(255.0f, 255.0f, 255.0f, 255.0f) { }

void DirectionalLightComponent::operator=(const DirectionalLightComponent &light) noexcept {
	transform = light.transform;
	direction = light.direction;
	ambient = light.ambient;
	diffuse = light.diffuse;
}
