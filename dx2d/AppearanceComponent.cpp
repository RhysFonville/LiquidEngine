#include "AppearanceComponent.h"

AppearanceComponent::AppearanceComponent(const std::shared_ptr<MeshComponent> &mesh)
	: Component(Component::Type::AppearanceComponent), mesh(mesh) { }
AppearanceComponent::AppearanceComponent(const Material &material)
	: Component(Component::Type::AppearanceComponent), material(material) { }
	
AppearanceComponent::AppearanceComponent(const GraphicsPipeline &pipeline, const std::shared_ptr<MeshComponent> &mesh,
	const Material &material) : Component(Component::Type::AppearanceComponent), mesh(mesh), pipeline(pipeline),
	mesh_changed(true) { }

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

const MeshComponent & AppearanceComponent::get_mesh() const noexcept {
	return *mesh;
}

void AppearanceComponent::set_mesh(const std::shared_ptr<MeshComponent> &mesh, ComPtr<ID3D12Device> &device,
	ComPtr<ID3D12GraphicsCommandList> &command_list) noexcept {
	this->mesh = mesh;

	pipeline.input_assembler.remove_mesh(0);
	pipeline.input_assembler.add_mesh(mesh->mesh_data, device, command_list);

	mesh_changed = true;
}

bool AppearanceComponent::mesh_was_changed() const noexcept {
	bool mesh_changed_before = mesh_changed;
	mesh_changed = false;
	return mesh_changed_before;
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
