#pragma once
#include "Component.h"
class SpotlightComponent : public Component {
public:
	__declspec(align(16))
	struct ConstantBufferStruct {
		XMFLOAT3 direction;
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;
	};

public:
	SpotlightComponent();

	void operator=(const SpotlightComponent &light) noexcept;

	operator ConstantBufferStruct() const noexcept {
		return { direction, 255.0f / ambient, 255.0f / diffuse };
	}

	FVector3 direction;
	FVector4 ambient;
	FVector4 diffuse;

	static const Type component_type = Type::SpotlightComponent;
};

