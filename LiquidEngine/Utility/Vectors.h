#pragma once

#include <algorithm>
#include <DirectXMath.h>
#include <dxgi1_4.h>
#include "commonmacros.h"

using namespace DirectX;

template <ACCEPT_DIGIT_ONLY(typename T)>
class TVector2 {
public:
	T x = 0;
	T y = 0;

	TVector2() : x(T()), y(T()) {}
	TVector2(T v) : x{v}, y{v} {}
	TVector2(T vec[2]) : x(vec[0]), y(vec[1]) { }
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
		return (x == vector.x &&
			y == vector.y);
	}

	bool operator!=(const TVector2 &vector) const noexcept {
		return (x != vector.x &&
			y != vector.y);
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

template <ACCEPT_DIGIT_ONLY(typename T)>
class TVector3 {
public:
	T x = 0;
	T y = 0;
	T z = 0;

	TVector3() {}
	TVector3(T v) : x{v}, y{v}, z{v} {}
	TVector3(T vec[3]) : x(vec[0]), y(vec[1]), z(vec[2]) { }
	TVector3(const TVector3 &vector) : x(vector.x), y(vector.y), z(vector.z) { }
	constexpr TVector3(T x, T y, T z) : x(x), y(y), z(z) {}

	bool is_zero() const noexcept {
		return (x == 0 && y == 0 && z == 0);
	}

	TVector3<T> operator+(const TVector3<T> &vector) {
		return TVector3<T>{x+vector.x, y+vector.y, z+vector.z};
	}

	TVector3<T> operator-(const TVector3<T> &vector) noexcept {
		return TVector3<T>(x-vector.x, y-vector.y, z-vector.z);
	}

	TVector3<T> operator*(const TVector3<T> &vector) const noexcept {
		return TVector3<T>(x*vector.x, y*vector.y, z*vector.z);
	}

	TVector3<T> operator*(T n) const noexcept {
		return TVector3<T>(x*n, y*n, z*n);
	}

	TVector3<T> operator/(const TVector3<T> &vector) const noexcept {
		return TVector3<T>(x/vector.x, y/vector.y, z/vector.z);
	}

	TVector3<T> operator/(T n) const noexcept {
		return TVector3<T>(x/n, y/n, z/n);
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

	void operator=(const TVector3<T> &vector) noexcept {
		x = vector.x;
		y = vector.y;
		z = vector.z;
	}

	T & operator[](UCHAR index) {
		switch (index) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			default:
				throw std::exception("Index out of bounds when using [] operator on TVector3.");
		}
	}

	float magnitude() {
		return sqrtf(x*x + y*y + z*z);
	}
};

template <ACCEPT_DIGIT_ONLY(typename T)>
class TVector4 {
public:
	T x = 0;
	T y = 0;
	T z = 0;
	T w = 0;

	TVector4() {}
	TVector4(T v) : x{v}, y{v}, z{v}, w{v} {}
	TVector4(T vec[4]) : x(vec[0]), y(vec[1]), z(vec[2]), w(vec[3]) { }
	constexpr TVector4(T x, T y, T z, T w) : x(x), y(y), z(z), w(w) { }
	constexpr TVector4(TVector3<T> vector, T w) : x(vector.x), y(vector.y), z(vector.z), w(w) { }

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
		TVector4<T> ret = *this;
		ret.x -= vector.x;
		ret.y -= vector.y;
		ret.z -= vector.z;
		ret.w -= vector.w;

		return ret;
	}

	TVector4<T> operator*(const TVector4<T> &vector) const noexcept {
		return TVector4<T>(x*vector.x, y*vector.y, z*vector.z, w*vector.w);
	}

	TVector4<T> operator*(T n) const noexcept {
		return TVector4<T>(x*n, y*n, z*n, w*n);
	}

	TVector4<T> operator/(const TVector4<T> &vector) const noexcept {
		return TVector4<T>(x/vector.x, y/vector.y, z/vector.z, w/vector.w);
	}

	TVector4<T> operator/(T n) const noexcept {
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

	void operator=(const TVector4<T> &vector) noexcept {
		x = vector.x;
		y = vector.y;
		z = vector.z;
		w = vector.w;
	}

	void operator=(const XMVECTOR &vector) noexcept {
		x = XMVectorGetX(vector);
		y = XMVectorGetY(vector);
		z = XMVectorGetZ(vector);
		w = XMVectorGetW(vector);
	}

	T & operator[](UCHAR index) {
		switch (index) {
			case 0:
				return x;
			case 1:
				return y;
			case 2:
				return z;
			case 3:
				return w;
			default:
				throw std::exception("Index out of bounds when using [] operator on TVector4.");
		}
	}


};

using Vector4 = TVector4<int>;
using Vector3 = TVector3<int>;
using Vector2 = TVector2<int>;
using Vector = Vector3;

class FVector3 : public TVector3<float> {
public:
	FVector3() : TVector3<float>() {}
	FVector3(TVector3<float> v) : TVector3<float>(v) {}
	FVector3(const FVector3 &vector)
		: TVector3<float>(vector) { }
	constexpr FVector3(float x, float y, float z)
		: TVector3<float>(x, y, z) { }
	FVector3(XMVECTOR vector)
		: TVector3<float>(XMVectorGetX(vector), XMVectorGetY(vector), XMVectorGetZ(vector)) { }
	FVector3(float* vector) : FVector3{vector[0], vector[1], vector[2]} { }

	operator TVector3<float>() const noexcept {
		return TVector3<float>(x, y, z);
	}

	operator XMVECTOR() const noexcept {
		return XMVectorSet(x, y, z, 1.0f);
	}

	operator XMFLOAT3() const noexcept {
		return { x, y, z };
	}

	FVector3 pow(float p) noexcept {
		return FVector3(powf(x, p), powf(y, p), powf(z, p));
	}

	float matrix_multiplication(const FVector3 &vector) const noexcept {
		return x*vector.x + y*vector.y + z*vector.z;
	}

	void operator=(const XMVECTOR &vector) noexcept {
		x = XMVectorGetX(vector);
		y = XMVectorGetY(vector);
		z = XMVectorGetZ(vector);
	}
};

class FVector4 : public TVector4<float> {
public:
	FVector4() {}
	FVector4(const FVector4 &vector)
		: TVector4<float>(vector) { }
	constexpr FVector4(float x, float y, float z, float w)
		: TVector4<float>(x, y, z, w) { }

	operator TVector4<float>() const noexcept {
		return FVector4(x, y, z, w);
	}

	operator XMVECTOR() const noexcept {
		return XMVectorSet(x, y, z, w);
	}

	operator XMFLOAT4() const noexcept {
		return { x, y, z, w };
	}

	FVector4 operator/(float f) const noexcept {
		return FVector4(x/f, y/f, z/f, w/f);
	}

	FVector4 pow(float p) noexcept {
		return FVector4(powf(x, p), powf(y, p),
			powf(z, p), powf(w, p));
	}

	float matrix_multiplication(const FVector4 &vector) const noexcept {
		return x*vector.x + y*vector.y + z*vector.z + w*vector.w;
	}
};

template<ACCEPT_DIGIT_ONLY(typename T)>
struct TColor {
	T r = 255, g = 255, b = 255, a = 255;

	TColor(T r, T g, T b, T a = 255) : r(r), g(g), b(b), a(a) { }

	operator XMFLOAT3() const noexcept {
		return XMFLOAT3(r, g, b);
	}

	operator XMFLOAT4() const noexcept {
		return XMFLOAT4(r, g, b, a);
	}

	operator DXGI_RGBA() const noexcept {
		return DXGI_RGBA({ r, g, b, a });
	}

	operator FVector4() const noexcept {
		return FVector4(r, g, b, a);
	}

	FVector4 to_vec_normalized() const noexcept {
		return (FVector4)*this / 255.0f;
	}

	TColor operator/(T divisor) const noexcept {
		return TColor(
			(T)(r / divisor), (T)(g / divisor),
			(T)(b / divisor), (T)(a / divisor)
		);
	}

	void operator/=(T divisor) noexcept {
		*this = *this / divisor;
	}

	bool operator==(const TColor<T> &color) const noexcept {
		return (r == color.r &&
			g == color.g &&
			b == color.b);
	}
};

using Color = TColor<UCHAR>;
using FColor = TColor<float>;

using FVector2 = TVector2<float>;

using UVector4 = TVector4<UINT>;
using UVector3 = TVector3<UINT>;
using UVector2 = TVector2<UINT>;
using UVector = Vector3;

static constexpr FVector3 global_forward { 0.0f,  0.0f,  1.0f };
static constexpr FVector3 global_backward{ 0.0f,  0.0f, -1.0f };
static constexpr FVector3 global_left    {-1.0f,  0.0f,  0.0f };
static constexpr FVector3 global_right   { 1.0f,  0.0f,  0.0f };
static constexpr FVector3 global_up      { 0.0f,  1.0f,  0.0f };
static constexpr FVector3 global_down    { 0.0f, -1.0f,  0.0f };

static XMFLOAT4 color_to_xmfloat4(const Color &color, bool normalize = true) noexcept {
	if (normalize)
		return XMFLOAT4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
	else
		return XMFLOAT4(color.r, color.g, color.b, color.a);
}

static XMFLOAT4 ctoxmf4(const Color &color, bool normalize = true) noexcept {
	return color_to_xmfloat4(color, normalize);
}

static FVector4 color_to_fvector(const Color &color, bool normalize = true) noexcept {
	return FVector4{(float)color.r / 255.0f, (float)color.g / 255.0f,
		(float)color.b / 255.0f, (float)color.a / 255.0f};
}

static FVector4 ctofvec(const Color &color) noexcept {
	return color_to_fvector(color);
}

static FVector2 UVector2_to_FVector2(const UVector2 &vector) noexcept {
	return FVector2((float)vector.x, (float)vector.y);
}
