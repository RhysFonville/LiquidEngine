#pragma once

#include <variant>
#include "../Mesh.h"

namespace GraphicsPipelineMeshChange {

/**
 * Addition or removal information.
 */
struct Command {
	enum class Type {
		Add, Sub
	};
	Type type;
	std::variant<std::pair<Mesh, size_t>, size_t> change; 
};

/**
* Stores additions and removals of meshes in the pipeline. Communicates these changes from the StaticMeshComponent to the GraphicsPipeline.
* \see StaticMeshComponent
* \see GraphicsPipeline
* \see Mesh
*/
class Manager {
public:
	Manager() { }

	void add_mesh(const Mesh &mesh, size_t index = (size_t)-1);
	void remove_mesh(size_t index);
	void remove_all_meshes();

	 std::vector<Command> get_changes(bool clear_changes);
	void clear_change_info();

private:
	std::vector<Command> changes;
};

};
