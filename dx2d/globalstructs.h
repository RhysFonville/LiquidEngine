#pragma once

#include <memory>
#include <vector>
#include <Windows.h>
#include <DirectXMath.h>

using namespace DirectX;

enum class RotationUnit {
	Radians,
	Degrees
};

class Object;
using ObjectVector = std::shared_ptr<std::vector<std::shared_ptr<Object>>>;

struct Color {
	UCHAR r = 255, g = 255, b = 255, a = 255;

	operator XMFLOAT4() {
		return XMFLOAT4(r, g, b, a);
	}
};

struct FColor {
	float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;

	FColor operator*(float n) const noexcept {
		return { r*n, g*n, b*n, a*n };
	}

	FColor operator/(float n) const noexcept {
		return { r/n, g/n, b/n, a/n };
	}

	operator Color() const noexcept {
		return { (UCHAR)r, (UCHAR)g, (UCHAR)b, (UCHAR)a };
	}
};

template <typename T>
struct TVector2 {
	T x = 0;
	T y = 0;

	TVector2() : x(T()), y(T()) {}
	TVector2(T x, T y) : x(x), y(y) {}
	TVector2(XMVECTOR vector) : x((T)XMVectorGetX(vector)), y((T)XMVectorGetY(vector)) { }

	bool is_zero() const noexcept {
		return (x == 0 && y == 0);
	}

	TVector2 operator+(TVector2 vector) {
		TVector2 ret = vector;
		ret.x += x;
		ret.y += y;
		return ret;
	}

	void operator+=(TVector2 vector) {
		x += vector.x;
		y += vector.y;
	}

	bool operator==(const TVector2 &vector) const noexcept {
		if (x == vector.x &&
			y == vector.y)
			return true;
		else
			return false;
	}

	bool operator!=(const TVector2 &vector) const noexcept {
		if (x != vector.x &&
			y != vector.y)
			return true;
		else
			return false;
	}

	operator XMVECTOR() const {
		return XMVectorSet(x, y, 0.0f, 1.0f);
	}

	operator XMFLOAT2() const noexcept {
		return { (float)x, (float)y };
	}

	void operator=(const TVector2<T> &vector) noexcept {
		x = vector.x;
		y = vector.y;
	}
};

template <typename T>
struct TVector3 {
	T x = 0;
	T y = 0;
	T z = 0;

	TVector3() {}
	constexpr TVector3(T x, T y, T z) : x(x), y(y), z(z) {}
	TVector3(XMVECTOR vector) : x((T)XMVectorGetX(vector)),
		y((T)XMVectorGetY(vector)), z((T)XMVectorGetZ(vector)) { }

	bool is_zero() const noexcept {
		return (x == 0 && y == 0 && z == 0);
	}

	TVector3<T> operator+(TVector3<T> vector) {
		vector.x += x;
		vector.y += y;
		vector.z += z;
		return vector;
	}

	TVector3<T> operator-(const TVector3<T> &vector) noexcept {
		TVector3<T> ret = *this;
		ret.x -= vector.x;
		ret.y -= vector.y;
		ret.z -= vector.z;

		return ret;
	}

	T matrix_multiplication(const TVector3<T> &vector) const noexcept {
		return this->x*vector.x + this->y*vector.y + this->z*vector.z;
	}

	TVector3<T> operator*(const TVector3<T> &vector) const noexcept {
		return TVector3<T>(x*vector.x, y*vector.y, z*vector.z);
	}

	TVector3<T> operator*(T n) const noexcept {
		return TVector3<T>(x*n, y*n, z*n);
	}

	void operator+=(const TVector3<T> &vector) noexcept {
		x += vector.x;
		y += vector.y;
		z += vector.z;
	}

	void operator-=(const TVector3<T> &vector) noexcept {
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
	}

	bool operator==(const TVector3<T> &vector) const noexcept {
		return (x == vector.x && y == vector.y && z == vector.z);
	}

	bool operator!=(const TVector3<T> &vector) const noexcept {
		return (x != vector.x || y != vector.y || z != vector.z);
	}

	operator XMVECTOR() const noexcept {
		return XMVectorSet((float)x, (float)y, (float)z, 1.0f);
	}

	operator XMFLOAT3() const noexcept {
		return { (float)x, (float)y, (float)z };
	}

	void operator=(const TVector3<T> &vector) noexcept {
		x = vector.x;
		y = vector.y;
		z = vector.z;
	}
};

template <typename T>
struct TVector4 {
	T x = 0;
	T y = 0;
	T z = 0;
	T w = 0;

	TVector4() {}
	constexpr TVector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { }

	bool is_zero() const noexcept {
		return (x == 0 && y == 0 && z == 0 && w == 0);
	}

	TVector4<T> operator+(TVector4<T> vector) {
		vector.x += x;
		vector.y += y;
		vector.z += z;
		vector.w += w;
		return vector;
	}

	TVector4<T> operator-(const TVector4<T> &vector) noexcept {
		TVector3<T> ret = *this;
		ret.x -= vector.x;
		ret.y -= vector.y;
		ret.z -= vector.z;

		return ret;
	}

	T matrix_multiplication(const TVector4<T> &vector) const noexcept {
		return this->x*vector.x + this->y*vector.y + this->z*vector.z + this->w*vector.w;
	}

	TVector4<T> operator*(const TVector4<T> &vector) const noexcept {
		return TVector4<T>(x*vector.x, y*vector.y, z*vector.z, w*vector.w);
	}

	TVector4<T> operator*(T n) const noexcept {
		return TVector4<T>(x*n, y*n, z*n, w*n);
	}

	TVector4<T> operator-(T n) const noexcept {
		return TVector4<T>(x/n, y/n, z/n, w/n);
	}

	void operator+=(const TVector4<T> &vector) noexcept {
		x += vector.x;
		y += vector.y;
		z += vector.z;
		w += vector.w;
	}

	void operator-=(const TVector4<T> &vector) noexcept {
		x -= vector.x;
		y -= vector.y;
		z -= vector.z;
		w -= vector.w;
	}

	bool operator==(const TVector4<T> &vector) const noexcept {
		return (x == vector.x && y == vector.y && z == vector.z && w == vector.w);
	}

	bool operator!=(const TVector4<T> &vector) const noexcept {
		return (x != vector.x || y != vector.y || z != vector.z || w != vector.w);
	}

	operator XMVECTOR() const noexcept {
		return XMVectorSet((float)x, (float)y, (float)z, 1.0f);
	}

	operator XMFLOAT4() const noexcept {
		return { (float)x, (float)y, (float)z, (float)w };
	}

	void operator=(const TVector4<T> &vector) noexcept {
		x = vector.x;
		y = vector.y;
		z = vector.z;
		w = vector.w;
	}
};

using Vector4 = TVector4<int>;
using Vector3 = TVector3<int>;
using Vector2 = TVector2<int>;
using Vector = Vector3;

using FVector4 = TVector4<float>;
using FVector3 = TVector3<float>;
using FVector2 = TVector2<float>;
using FVector = FVector3;

using FPosition3 = FVector3;
using FPosition2 = FVector2;
using FPosition = FPosition3;

using FSize3 = FVector3;
using FSize2 = FVector2;
using FSize = FSize3;

using FRotation3 = FVector3;
using FRotation2 = FVector2;
using FRotation = FRotation3;

using Position3 = Vector3;
using Position2 = Vector2;
using Position = Position3;

using Size3 = Vector3;
using Size2 = Vector2;
using Size = Size3;

using FPoint3 = FPosition3;
using FPoint2 = FPosition2;
using FPoint = FPoint3;

using Point3 = Position3;
using Point2 = Position2;
using Point = Point3;

using Normal3 = FVector3;
using Normal2 = FVector2;
using Normal = Normal3;

using Tangent3 = FVector3;
using Tangent2 = FVector2;
using Tangent = Tangent3;

using Bitangent3 = FVector3;
using Bitangent2 = FVector2;
using Bitangent = Bitangent3;

using Binormal3 = FVector3;
using Binormal2 = FVector2;
using Binormal = Binormal3;

struct Transform {
	FPosition3 position = { 0, 0, 0 };
	FRotation3 rotation = { 0, 0, 0 };
	FSize3 size = { 1, 1, 1 };

	Transform()
		: position({ 0.0f, 0.0f, 0.0f }), rotation({ 0.0f, 0.0f, 0.0f }), size({ 1.0f, 1.0f, 1.0f }) { }
	Transform(FPosition3 position, FRotation3 rotation)
		: position(position), rotation(rotation), size({ 1.0f, 1.0f, 1.0f }) { }
	Transform(FPosition3 position, FRotation3 rotation, FSize3 size)
		: position(position), rotation(rotation), size(size) { }

	operator XMMATRIX() const noexcept {
		return DirectX::XMMatrixTranspose(
			DirectX::XMMatrixRotationX(rotation.x) *
			DirectX::XMMatrixRotationY(rotation.y) *
			DirectX::XMMatrixRotationZ(rotation.z) *
			DirectX::XMMatrixTranslation(position.x, position.y, position.z) *
			DirectX::XMMatrixScaling(size.x, size.y, size.z)
		);
	}

	bool operator==(const Transform &transform) const noexcept {
		if (position == transform.position &&
			rotation == transform.rotation &&
			size == transform.size)
			return true;
		else
			return false;
	}
};

template <typename T>
TVector3<T> operator*(T n, const TVector3<T> *vector) noexcept {
	return TVector3<T>(vector.x*n, vector.y*n, vector.z*n);
}

template <typename T>
TVector4<T> operator*(T n, const TVector4<T> &vector) noexcept {
	return TVector4<T>(vector.x*n, vector.y*n, vector.z*n, vector.w*n);
}

template <typename T>
TVector3<T> operator/(T n, const TVector3<T> *vector) noexcept {
	return TVector3<T>(vector.x/n, vector.y/n, vector.z/n);
}

template <typename T>
TVector4<T> operator/(T n, const TVector4<T> &vector) noexcept {
	return TVector4<T>(vector.x/n, vector.y/n, vector.z/n, vector.w/n);
}
