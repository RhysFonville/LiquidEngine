#pragma once

#include "../globalutil.h"
#include "../Controllable.h"

class Component : public Controllable {
public:
	enum class Type {
		None,
		StaticMeshComponent,
		CameraComponent,
		DirectionalLightComponent,
		PointLightComponent,
		SpotlightComponent,
		BoxComponent,
		SkyComponent
	};

	Component() : type(Type::None) { }
	Component(const Type &type);

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

	GET Type get_type() const noexcept;

	virtual bool operator==(const Component &component) const noexcept;
	virtual void operator=(const Component &component) noexcept;

	Component* parent = nullptr;

protected:
	Transform transform;
	Type type = Type::None;
};
