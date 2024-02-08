#include "GraphicsPipelineMeshChange.h"

namespace GraphicsPipelineMeshChange {

void Manager::add_mesh(const Mesh &mesh, size_t index) {
	changes.push_back(Command{Command::Type::Add, std::make_pair(mesh, index)});
}

void Manager::remove_mesh(size_t index) {
	changes.push_back(Command{Command::Type::Sub, index});
}

void Manager::remove_all_meshes() {
	remove_mesh((size_t)-1);
}

std::vector<Command> Manager::get_changes(bool clear_changes) {
	auto changes = this->changes;
	clear_change_info();
	return changes;
}

void Manager::clear_change_info() {
	changes.clear();
}

}
