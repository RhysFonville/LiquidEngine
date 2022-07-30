#pragma once
#include "Component.h"
class SpotlightComponent : public Component {
public:
	__declspec(align(16))
	struct ConstantBufferStruct {
		XMFLOAT3 direction;
		XMFLOAT4 ambient;
		XMFLOAT4 diffuse;

		ConstantBufferStruct() : direction(XMFLOAT3()), ambient(XMFLOAT4()), diffuse(XMFLOAT4()) {}

		ConstantBufferStruct(XMFLOAT3 dir, XMFLOAT4 amb, XMFLOAT4 diff)
			: direction(dir), ambient(amb), diffuse(diff) { }
	};

public:
	SpotlightComponent();

	void operator=(const SpotlightComponent &light) noexcept;

	operator ConstantBufferStruct() const noexcept {
		return ConstantBufferStruct(direction, 255.0f / ambient, 255.0f / diffuse);
	}

	FVector3 direction;
	FVector4 ambient;
	FVector4 diffuse;

	static const Type component_type = Type::SpotlightComponent;
};

