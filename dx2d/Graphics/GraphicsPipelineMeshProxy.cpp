#include "GraphicsPipelineMeshProxy.h"

void GraphicsPipelineMeshProxy::add_mesh(const Mesh &mesh, size_t index) {
	changes.additions.push_back(std::make_pair(mesh, index));
	changes.order.push_back(GraphicsPipelineMeshChangeInfoType::Add);
}

void GraphicsPipelineMeshProxy::remove_mesh(size_t index) {
	changes.subtractions.push_back(index);
	changes.order.push_back(GraphicsPipelineMeshChangeInfoType::Sub);
}

void GraphicsPipelineMeshProxy::remove_all_meshes() {
	changes.subtractions.push_back((size_t)-1);
	changes.order.push_back(GraphicsPipelineMeshChangeInfoType::Sub);
}

GraphicsPipelineMeshChangeInfo GraphicsPipelineMeshProxy::get_changes(bool clear_changes) {
	GraphicsPipelineMeshChangeInfo changes = this->changes;
	clear_change_info();
	return changes;
}

void GraphicsPipelineMeshProxy::clear_change_info() {
	changes.additions.clear();
	changes.subtractions.clear();
}
