#pragma once

#include <DirectXMath.h>
#include <concepts>
#include "../Utility/globalstructs.h"
#include "../Mesh.h"
#include "../Components/Component.h"
#include "../Controllable.h"

using namespace DirectX;

struct ObjectsTreeNode;
class Scene;

/**
 * Base class for all objects.
 */
class Object : public Controllable, public ComponentHolder {
public:
	Object() : ComponentHolder{} { }
	
	void clean_components();

	void base_clean_up() override;
	void base_compile() override;
	void base_tick(float dt) override;

	void set_position(const FVector3 &position) noexcept;
	void set_rotation(const FVector3 &rotation) noexcept;
	void set_size(const FVector3 &size) noexcept;

	void translate(const FVector3 &position) noexcept;
	void rotate(const FVector3 &rotation) noexcept;
	void size(const FVector3 &size) noexcept;

	GET FVector3 get_position() const noexcept;
	GET FVector3 get_rotation() const noexcept;
	GET FVector3 get_size() const noexcept;

	void set_transform(const Transform &transform) noexcept;
	GET Transform get_transform() const noexcept;

	GET Object* get_parent() noexcept;
	void set_parent(Object* parent) noexcept;

	void add_component(const std::shared_ptr<Component>& component, Component* parent = nullptr); //? AHHHHHHHHHHHHHHHHHHHHHHHH
	
	Object* add_object(std::unique_ptr<Object>&& obj);

	GET std::set<Object*> get_children() noexcept;

	void set_scene(Scene* scene) const noexcept;

	bool operator==(const Object &object) const noexcept;
	bool operator!=(const Object &object) const noexcept;

	void base_render_editor_gui_section(std::vector<ObjectsTreeNode>& nodes);
	void base_render_editor_gui_section() override;

	std::string name{""};

	Component* mimic_position_component{nullptr};
	Component* mimic_rotation_component{nullptr};
	Component* mimic_size_component{nullptr};

protected:
	Scene* scene{nullptr};

private:
	Transform transform{};

	Object* parent{nullptr};
	std::set<std::unique_ptr<Object>> children{};

	void remove_this_from_parents_children();
};

static int object_name_index = 0;

// For ImGUI
struct ObjectsTreeNode {
	Object* object;
	size_t child_index{};
	size_t child_count{};

	static void display_node(ObjectsTreeNode* node, std::vector<ObjectsTreeNode>& all_nodes) {
		static std::string clicked_name{};
		static std::string selected_name{};

		std::string name{node->object->name};
		if (node->object->name.empty()) {
			name = "Unnamed object ";
			name += std::to_string(object_name_index);
		}

		const bool is_folder = (node->child_count > 0);
		if (is_folder) {
			if (clicked_name == name) {
				ImGui::SetNextItemOpen(false);
				clicked_name = "";
			}
			if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanAllColumns | ImGuiTreeNodeFlags_OpenOnArrow)) {
				for (int child_n = 0; child_n < node->child_count; child_n++) {
					object_name_index++;
					display_node(&all_nodes[node->child_index + child_n], all_nodes);
				}
				ImGui::TreePop();
				clicked_name = name;
				selected_name = name;
			}
		} else {
			object_name_index++;

			if (clicked_name == name) {
				ImGui::SetNextItemOpen(false);
				clicked_name = "";
			}
			if (ImGui::TreeNodeEx(name.c_str(), ImGuiTreeNodeFlags_SpanAllColumns/* | ImGuiTreeNodeFlags_Leaf*/ | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen)) {
				clicked_name = name;
				selected_name = name;
			}
		}

		if (selected_name == name) {
			node->object->base_render_editor_gui_section();
		}
	}
};
