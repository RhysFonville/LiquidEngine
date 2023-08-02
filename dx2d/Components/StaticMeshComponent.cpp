#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent()
	: PhysicalComponent(Type::StaticMeshComponent) { }

void StaticMeshComponent::clean_up() { }

void StaticMeshComponent::compile() {
	mesh.compile();
}

bool StaticMeshComponent::operator==(const StaticMeshComponent &mesh) const noexcept {
	return ((Component*)this == (Component*)&mesh &&
		mesh == mesh.mesh);
}

void StaticMeshComponent::operator=(const StaticMeshComponent &component) noexcept {
	mesh = component.mesh;
}

