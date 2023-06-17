#include "AppearanceComponent.h"

AppearanceComponent::AppearanceComponent(MeshComponent* mesh)
	: Component(Component::Type::AppearanceComponent), mesh(mesh) { }
AppearanceComponent::AppearanceComponent(const Material &material)
	: Component(Component::Type::AppearanceComponent), material(material) { }
	
AppearanceComponent::AppearanceComponent(const GraphicsPipeline &pipeline, MeshComponent* mesh,
	const Material &material) : Component(Component::Type::AppearanceComponent), mesh(mesh),
	pipeline(pipeline) { }

void AppearanceComponent::compile(ComPtr<ID3D12Device> &device, ComPtr<ID3D12GraphicsCommandList> &command_list, const DXGI_SAMPLE_DESC &sample_desc, const UVector2 &resolution) noexcept {
	material.compile(pipeline);
	
	pipeline.compile(device, sample_desc, resolution);
	
	if (mesh != nullptr) {
		pipeline.input_assembler.add_mesh(mesh->mesh_data, device, command_list);
	}
}

void AppearanceComponent::clean_up() {
	pipeline.clean_up();
}

const MeshComponent* AppearanceComponent::get_mesh() const noexcept {
	return mesh;
}

void AppearanceComponent::set_mesh(MeshComponent* mesh, ComPtr<ID3D12Device> &device,
	ComPtr<ID3D12GraphicsCommandList> &command_list) noexcept {
	this->mesh = mesh;

	pipeline.input_assembler.remove_mesh(0);
	pipeline.input_assembler.add_mesh(mesh->mesh_data, device, command_list);
}

bool AppearanceComponent::operator==(const AppearanceComponent &appearance) const noexcept {
	return ((Component*)this == (Component*)&appearance &&
		mesh == appearance.mesh &&
		pipeline == appearance.pipeline);
}

void AppearanceComponent::operator=(const AppearanceComponent &component) noexcept {
	mesh = component.mesh;
	pipeline = component.pipeline;
	material = component.material;
}
