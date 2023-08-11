#include "GraphicsPipelineMeshChangeManager.h"

void GraphicsPipelineMeshChangeManager::add_mesh(const Mesh &mesh, size_t index) {
	changes.additions.push_back(std::make_pair(mesh, index));
	changes.order.push_back(GraphicsPipelineMeshChangeInfoType::Add);
}

void GraphicsPipelineMeshChangeManager::remove_mesh(size_t index) {
	changes.subtractions.push_back(index);
	changes.order.push_back(GraphicsPipelineMeshChangeInfoType::Sub);
}

void GraphicsPipelineMeshChangeManager::remove_all_meshes() {
	changes.subtractions.push_back((size_t)-1);
	changes.order.push_back(GraphicsPipelineMeshChangeInfoType::Sub);
}

GraphicsPipelineMeshChangeInfo GraphicsPipelineMeshChangeManager::get_changes(bool clear_changes) {
	GraphicsPipelineMeshChangeInfo changes = this->changes;
	clear_change_info();
	return changes;
}

void GraphicsPipelineMeshChangeManager::clear_change_info() {
	changes.additions.clear();
	changes.subtractions.clear();
	changes.order.clear();
}
