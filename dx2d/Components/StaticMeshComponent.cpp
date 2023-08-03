#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent() : PhysicalComponent(Type::StaticMeshComponent) {
	material.pipeline.input_assembler.set_proxy(proxy);
}

void StaticMeshComponent::compile(ComPtr<ID3D12Device> &device, ComPtr<ID3D12GraphicsCommandList> &command_list,
	const DXGI_SAMPLE_DESC &sample_desc, const UVector2 &resolution) {

	material.compile(device, command_list, sample_desc, resolution);

	mesh.compile();
	proxy->add_mesh(mesh);
}

void StaticMeshComponent::clean_up() {
	material.clean_up();
}

Mesh StaticMeshComponent::get_mesh() const noexcept {
	return mesh;
}

void StaticMeshComponent::set_mesh(const Mesh &mesh) noexcept {
	this->mesh = mesh;

	proxy->remove_mesh(0);
	proxy->add_mesh(mesh);
}

bool StaticMeshComponent::operator==(const StaticMeshComponent &mesh) const noexcept {
	return ((Component*)this == (Component*)&mesh &&
		this->mesh == mesh.mesh);
}

void StaticMeshComponent::operator=(const StaticMeshComponent &component) noexcept {
	mesh = component.mesh;
}

