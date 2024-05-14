#include "DirectionalLightComponent.h"

DirectionalLightComponent::DirectionalLightComponent()
	: LightComponent(Type::DirectionalLightComponent) {
	set_rotation(FVector3{90.0f, 180.0f, -360.0f});
}
