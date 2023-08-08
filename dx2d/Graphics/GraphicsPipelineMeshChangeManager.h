#pragma once

#include "../Mesh.h"

enum class GraphicsPipelineMeshChangeInfoType {
	Add, Sub
};

struct GraphicsPipelineMeshChangeInfo {
	std::vector<std::pair<Mesh, size_t>> additions;
	std::vector<size_t> subtractions;
	std::vector<GraphicsPipelineMeshChangeInfoType> order;
};

class GraphicsPipelineMeshChangeManager {
public:
	GraphicsPipelineMeshChangeManager() { }

	void add_mesh(const Mesh &mesh, size_t index = (size_t)-1);
	void remove_mesh(size_t index);
	void remove_all_meshes();

	GraphicsPipelineMeshChangeInfo get_changes(bool clear_changes);
	void clear_change_info();

private:
	GraphicsPipelineMeshChangeInfo changes;
};
