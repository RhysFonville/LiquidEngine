#pragma once

#include "globalutil.h"

class Component {
public:
	enum class Type {
		MeshComponent,
		CameraComponent,
		DirectionalLightComponent,
		PointLightComponent,
		SpotlightComponent
	};

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

	virtual void clean_up() { };

	virtual void compile() { };

	bool operator==(const Component &component) const noexcept;

	const Type type;

protected:
	Transform transform;
};

