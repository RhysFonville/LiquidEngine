#pragma once

#include <variant>
#include <optional>
#include "../Mesh.h"
#include "../Utility/ObjectStructs.h"
#include "GraphicsPipeline.h"

// https://refactoring.guru/design-patterns/command

/**
 * Command for the input assembler.
 * \see GraphicsPipeline
 */
struct GraphicsPipelineIACommand {
	enum class Type {
		AddMesh,
		RemoveMesh,
		RemoveAll,
		SetInstances
	};
	
	GraphicsPipelineIACommand(Type type) : type{type} { }

	Type get_type() const noexcept { return type; }

public:
	Type type{};
};

/**
 * Add a mesh.
 * \see Mesh
 */
struct GraphicsPipelineIAAddMeshCommand : public GraphicsPipelineIACommand {
	GraphicsPipelineIAAddMeshCommand(const std::shared_ptr<const Mesh>& mesh)
		: GraphicsPipelineIACommand{Type::AddMesh}, mesh{mesh} { }
	GraphicsPipelineIAAddMeshCommand(const std::shared_ptr<const Mesh>& mesh, size_t index)
		: GraphicsPipelineIACommand{Type::AddMesh}, mesh{mesh}, index{index} { }

	const std::shared_ptr<const Mesh>& get_mesh() const noexcept {
		return mesh;
	}

	const std::optional<size_t>& get_index() const noexcept {
		return index;
	}

private:
	const std::shared_ptr<const Mesh> mesh;
	std::optional<size_t> index{std::nullopt};
};

/**
 * Remove a mesh.
 * \see Mesh
 */
struct GraphicsPipelineIARemoveMeshCommand : public GraphicsPipelineIACommand {
	GraphicsPipelineIARemoveMeshCommand(size_t index)
		: GraphicsPipelineIACommand{Type::RemoveMesh}, index{index} { }

	size_t get_index() const noexcept {
		return index;
	}

private:
	size_t index{};
};

/**
 * Remove all meshes.
 * \see Mesh
 */
struct GraphicsPipelineIARemoveAllMeshesCommand : public GraphicsPipelineIACommand {
	GraphicsPipelineIARemoveAllMeshesCommand()
		: GraphicsPipelineIACommand{Type::RemoveAll} { }
};

/**
 * Set instances vector.
 */
struct GraphicsPipelineIASetInstancesCommand : public GraphicsPipelineIACommand {
	GraphicsPipelineIASetInstancesCommand(const std::vector<Transform>& instances)
		: GraphicsPipelineIACommand{Type::SetInstances}, instances{instances} { }

	const std::vector<Transform> & get_instances() const noexcept {
		return instances;
	}

private:
	std::vector<Transform> instances{};
};
