#include "StaticMeshComponent.h"

StaticMeshComponent::StaticMeshComponent(Mesh mesh)
	: GraphicsComponent(Type::StaticMeshComponent), mesh(mesh) {
	material->pipeline.input_assembler.set_proxy(proxy);
}

void StaticMeshComponent::compile(const ComPtr<ID3D12Device> &device, const ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const D3D12_DEPTH_STENCIL_DESC &depth_stencil_desc, const UVector2 &resolution) noexcept {
	mesh.compile();
	proxy->add_mesh(mesh);
	material->compile(device, command_list, sample_desc, depth_stencil_desc, resolution);
}

void StaticMeshComponent::compile() noexcept {
	mesh.compile();
	proxy->add_mesh(mesh);
	material->compile();
}

void StaticMeshComponent::clean_up() {
	material->clean_up();
}

const Mesh & StaticMeshComponent::get_mesh() noexcept {
	return mesh;
}

void StaticMeshComponent::set_mesh(const Mesh &mesh) noexcept {
	this->mesh = mesh;
	this->mesh.compile();

	proxy->remove_all_meshes();
	proxy->add_mesh(mesh);

	changed = true;
}

MaterialComponent* StaticMeshComponent::get_material() const noexcept {
	return material;
}

void StaticMeshComponent::set_material(MaterialComponent* material) noexcept {
	this->material = material;
	this->material->pipeline.input_assembler.set_proxy(proxy);
	proxy->remove_all_meshes();
	proxy->add_mesh(mesh);

	changed = true;
}

bool StaticMeshComponent::operator==(const StaticMeshComponent &mesh) const noexcept {
	return ((Component*)this == (Component*)&mesh &&
		this->mesh == mesh.mesh);
}

void StaticMeshComponent::operator=(const StaticMeshComponent &component) noexcept {
	mesh = component.mesh;
}

