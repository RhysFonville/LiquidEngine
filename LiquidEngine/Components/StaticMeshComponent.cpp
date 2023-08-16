#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent() : PhysicalComponent(Type::StaticMeshComponent) {
	material.pipeline.input_assembler.set_proxy(proxy);
}

void StaticMeshComponent::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const UVector2 &resolution) noexcept {
	material.compile(device, command_list, sample_desc, resolution);

	mesh.compile();
	proxy->add_mesh(mesh);
}

void StaticMeshComponent::compile() noexcept {
	material.compile();

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
	this->mesh.compile();

	proxy->add_mesh(mesh);
}

Material & StaticMeshComponent::get_material() noexcept {
	return material;
}

void StaticMeshComponent::set_material(const Material &material) noexcept {
	this->material = material;
	this->material.compile();

	this->material.pipeline.input_assembler.set_proxy(proxy);
	proxy->remove_all_meshes();
	proxy->add_mesh(mesh);
}

bool StaticMeshComponent::operator==(const StaticMeshComponent &mesh) const noexcept {
	return ((Component*)this == (Component*)&mesh &&
		this->mesh == mesh.mesh);
}

void StaticMeshComponent::operator=(const StaticMeshComponent &component) noexcept {
	mesh = component.mesh;
}

