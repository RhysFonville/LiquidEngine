#pragma once

#include <DirectXMath.h>
#include "ObjectAppearance.h"

using namespace DirectX;

using FRotation = FVector3;
using FScale = FVector3;

struct Transform {
	FPosition position = { 0, 0, 0 };
	FRotation rotation = { 0, 0, 0 };
	FScale scale = { 1, 1, 1 };

	operator XMMATRIX() const {
		return DirectX::XMMatrixTranspose(
			DirectX::XMMatrixRotationX(rotation.x) *
			DirectX::XMMatrixRotationY(rotation.y) *
			DirectX::XMMatrixRotationZ(rotation.z) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z) *
			DirectX::XMMatrixPerspectiveLH(1.0f, 3.0f / 4.0f, 0.5f, 10.0f)
		);
	}
};

class Object {
public:
	Object();
	Object(ObjectAppearance appearance);

	ObjectAppearance appearance;
	Transform transform;
};

