#include "DirectionalLightComponent.h"

DirectionalLightComponent::DirectionalLightComponent() : LightComponent{} {
	set_rotation(FVector3{90.0f, 180.0f, -360.0f});
}
