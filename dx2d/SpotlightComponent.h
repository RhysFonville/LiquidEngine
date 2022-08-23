#pragma once
#include "Component.h"
class SpotlightComponent : public Component {
public:
	__declspec(align(16))
	struct ConstantBufferStruct {
		XMFLOAT3 direction;
		XMFLOAT4 diffuse;
		XMFLOAT4 specular;
	};

public:
	SpotlightComponent();

	void operator=(const SpotlightComponent &light) noexcept;

	operator ConstantBufferStruct() const noexcept {
		return { direction, 255.0f / diffuse, specular };
	}

	FVector3 direction;
	FVector4 diffuse;
	FVector4 specular;

	static const Type component_type = Type::SpotlightComponent;
};

