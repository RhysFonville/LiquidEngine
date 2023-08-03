#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent() : PhysicalComponent(Type::StaticMeshComponent) { }

void StaticMeshComponent::compile() {
	mesh.compile();

	material.compile(pipeline);

	pipeline.compile(device, sample_desc, resolution);

	if (mesh != nullptr) {
		pipeline.input_assembler.add_mesh(mesh->mesh_data, device, command_list);
	}
}

void StaticMeshComponent::clean_up() {
	pipeline.clean_up();
}

Mesh StaticMeshComponent::get_mesh() const noexcept {
	return mesh;
}

void StaticMeshComponent::set_mesh(Mesh &mesh) noexcept {
	this->mesh = mesh;

	proxy.remove_mesh(0);
	proxy.add_mesh(mesh);
}

bool StaticMeshComponent::operator==(const StaticMeshComponent &mesh) const noexcept {
	return ((Component*)this == (Component*)&mesh &&
		mesh == mesh.mesh);
}

void StaticMeshComponent::operator=(const StaticMeshComponent &component) noexcept {
	mesh = component.mesh;
}

