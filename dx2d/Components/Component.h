#pragma once

#include "../globalutil.h"

class Component {
public:
	enum class Type {
		None,
		StaticMeshComponent,
		CameraComponent,
		DirectionalLightComponent,
		PointLightComponent,
		SpotlightComponent,
	};

	Component() : type(Type::None) { }
	Component(const Type &type, const Transform &transform = Transform());

	virtual void set_position(FVector3 position) noexcept;
	virtual void set_rotation(FVector3 rotation) noexcept;
	virtual void set_size(FVector3 size) noexcept;

	virtual void translate(FVector3 position) noexcept;
	virtual void rotate(FVector3 rotation) noexcept;
	virtual void size(FVector3 size) noexcept;

	virtual GET FVector3 get_position() const noexcept;
	virtual GET FVector3 get_rotation() const noexcept;
	virtual GET FVector3 get_size() const noexcept;

	virtual void set_transform(Transform transform) noexcept;
	virtual GET Transform get_transform() const noexcept;

	virtual void clean_up() { }
	virtual void compile() { }
	virtual void tick() { }

	GET Type get_type() const noexcept;

	virtual bool operator==(const Component &component) const noexcept;
	virtual void operator=(const Component &component) noexcept;

	GET bool has_component(Component::Type search) const noexcept;
	template <typename T>
	GET bool has_component() const {
		for (const std::shared_ptr<Component> &component : components) {
			if (component->get_type() == T::component_type) {
				return true;
			}
		}
		return false;
	}

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	GET T* get_component() noexcept {
		for (std::shared_ptr<Component> &component : components) {
			if (component->get_type() == T::component_type) {
				return (T*)component.get();
			}
		}
		return nullptr;
	}

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	GET std::vector<T*> get_components() noexcept {
		std::vector<T*> ret;
		for (std::shared_ptr<Component> &component : components) {
			if (component->get_type() == T::component_type) {
				ret.push_back((T*)component.get());
			}
		}
		return ret;
	}

	template <ACCEPT_BASE_AND_HEIRS_ONLY(typename T, Component)>
	void add_component(T component) { //? AHHHHHHHHHHHHHHHHHHHHHHHH
		components.push_back(std::make_shared<T>(component));
		components.back().lock()->parent = this;
	}

	void remove_component(size_t index) {
		components.erase(components.begin()+index);
	}

	Component* parent;
	std::vector<std::shared_ptr<Component>> components; //! HAS TO BE POINTER SO WE CAN CAST TO SUBCLASSES

protected:
	Transform transform;
	Type type;
};
