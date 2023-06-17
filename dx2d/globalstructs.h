#pragma once

#include <memory>
#include <vector>
#include <Windows.h>
#include <DirectXMath.h>
#include <utility>
#include <dxgi1_4.h>

using namespace DirectX;

#define ACCEPT_DIGIT_ONLY(T) T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type

enum class RotationUnit {
	Radians,
	Degrees
};

template<ACCEPT_DIGIT_ONLY(typename T)>
struct TColor {
	T r = 255, g = 255, b = 255, a = 255;

	TColor(T r, T g, T b, T a = 255) : r(r), g(g), b(b), a(a) { }

	operator XMFLOAT3() {
		return XMFLOAT3(r, g, b);
	}

	operator XMFLOAT4() {
		return XMFLOAT4(r, g, b, a);
	}

	operator DXGI_RGBA() {
		return DXGI_RGBA({ r, g, b, a });
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

static XMFLOAT4 color_to_xmfloat4(const Color &color, bool normalize = true) noexcept {
	if (normalize)
		return XMFLOAT4(color.r / 255.0f, color.g / 255.0f, color.b / 255.0f, color.a / 255.0f);
	else
		return XMFLOAT4(color.r, color.g, color.b, color.a);
}

static XMFLOAT4 ctoxmf4(const Color &color, bool normalize = true) noexcept {
	return color_to_xmfloat4(color, normalize);
}

//struct FColor {
//	float r = 1.0f, g = 1.0f, b = 1.0f, a = 1.0f;
//
//	FColor operator*(float n) const noexcept {
//		return { r*n, g*n, b*n, a*n };
//	}
//
//	FColor operator/(float n) const noexcept {
//		return { r/n, g/n, b/n, a/n };
//	}
//
//	operator Color() const noexcept {
//		return { (UCHAR)r, (UCHAR)g, (UCHAR)b, (UCHAR)a };
//	}
//};

template <ACCEPT_DIGIT_ONLY(typename T)>
class TVector2 {
public:
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

template <typename T>
class TVector3 {
public:
	T x = 0;
	T y = 0;
	T z = 0;

	TVector3() {}
	TVector3(const TVector3 &vector) : x(vector.x), y(vector.y), z(vector.z) { }
	constexpr TVector3(T x, T y, T z) : x(x), y(y), z(z) {}

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
};

template <typename T>
class TVector4 {
public:
	T x = 0;
	T y = 0;
	T z = 0;
	T w = 0;

	TVector4() {}
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

static FVector4 color_to_fvector(const Color &color) noexcept {
	return FVector4(color.r, color.g, color.b, color.a);
}

static FVector4 ctofvec(const Color &color) noexcept {
	return color_to_fvector(color);
}

using FVector2 = TVector2<float>;

using UVector4 = TVector4<UINT>;
using UVector3 = TVector3<UINT>;
using UVector2 = TVector2<UINT>;
using UVector = Vector3;

struct Transform {
	FVector3 position = { 0, 0, 0 };
	FVector3 rotation = { 0, 0, 0 };
	FVector3 size = { 1, 1, 1 };

	Transform()
		: position({ 0.0f, 0.0f, 0.0f }), rotation({ 0.0f, 0.0f, 0.0f }), size({ 1.0f, 1.0f, 1.0f }) { }
	Transform(FVector3 position, FVector3 rotation)
		: position(position), rotation(rotation), size({ 1.0f, 1.0f, 1.0f }) { }
	Transform(FVector3 position, FVector3 rotation, FVector3 size)
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
		return (position == transform.position &&
			rotation == transform.rotation &&
			size == transform.size);
	}
};

struct SimpleVertex {
	FVector3 position = FVector3();

	SimpleVertex() { }

	SimpleVertex(float x, float y, float z) : position(FVector3(x, y, z)) { }

	SimpleVertex(FVector3 position)
		: position(position) { }

	bool operator==(const SimpleVertex &vertex) const noexcept {
		return (position == vertex.position);
	}

	bool operator!=(const SimpleVertex &vertex) const noexcept {
		return (position != vertex.position);
	}
};

struct Vertex {
	FVector3 position = FVector3();
	FVector2 texcoord = FVector2();
	FVector3 normal = FVector3();
	FVector3 tangent = FVector3();
	//FVector3 bitangent = FVector3();

	Vertex() { }

	Vertex(float x, float y, float z) : position(x, y, z) { }

	Vertex(float x, float y, float z, float u, float v)
		: position(x, y, z), texcoord(u, v) { }

	Vertex(float x, float y, float z,
		float u, float v,
		float nx, float ny, float nz)
		: position(x, y, z), texcoord(u, v), normal(nx, ny, nz) { }

	Vertex(FVector3 position, FVector2 texcoord, FVector3 normal)
		: position(position), texcoord(texcoord), normal(normal) { }

	bool operator==(const Vertex &vertex) const noexcept {
		return (position == vertex.position &&
			texcoord == vertex.texcoord &&
			normal == vertex.normal &&
			tangent == vertex.tangent);
	}

	operator SimpleVertex() const noexcept {
		return SimpleVertex(position);
	}
};

struct SimpleTriangle {
	SimpleVertex first;
	SimpleVertex second;
	SimpleVertex third;

	bool contains(const SimpleVertex &vertex) {
		return (first == vertex || second == vertex || third == vertex);
	}
};

struct Triangle {
	Vertex first;
	Vertex second;
	Vertex third;

	operator SimpleTriangle() const noexcept {
		return SimpleTriangle((SimpleVertex)first, (SimpleVertex)second,
			(SimpleVertex)third);
	}
};

struct SimpleBox {
	std::vector<SimpleVertex> vertices;

	SimpleBox() : vertices(std::vector<SimpleVertex>(8)) { }
	SimpleBox(const std::vector<SimpleVertex> &verts) {
		vertices.reserve(8);

		auto x_extremes = std::minmax_element(verts.begin(), verts.end(),
			[](const SimpleVertex &lhs, const SimpleVertex &rhs) {
				return lhs.position.x < rhs.position.x;
			});

		auto y_extremes = std::minmax_element(verts.begin(), verts.end(),
			[](const SimpleVertex &lhs, const SimpleVertex &rhs) {
				return lhs.position.y < rhs.position.y;
			});

		auto z_extremes = std::minmax_element(verts.begin(), verts.end(),
			[](const SimpleVertex &lhs, const SimpleVertex &rhs) {
				return lhs.position.z < rhs.position.z;
			});

		SimpleVertex v1(x_extremes.first->position.x,
			y_extremes.first->position.y,
			z_extremes.first->position.z
		);
		SimpleVertex v2(x_extremes.first->position.x,
			y_extremes.second->position.y,
			z_extremes.first->position.z
		);
		SimpleVertex v3(x_extremes.second->position.x,
			y_extremes.second->position.y,
			z_extremes.first->position.z
		);
		SimpleVertex v4(x_extremes.second->position.x,
			y_extremes.first->position.y,
			z_extremes.first->position.z
		);
		SimpleVertex v5(x_extremes.first->position.x,
			y_extremes.first->position.y,
			z_extremes.second->position.z
		);
		SimpleVertex v6(x_extremes.first->position.x,
			y_extremes.second->position.y,
			z_extremes.second->position.z
		);
		SimpleVertex v7(x_extremes.second->position.x,
			y_extremes.second->position.y,
			z_extremes.second->position.z
		);
		SimpleVertex v8(x_extremes.second->position.x,
			y_extremes.first->position.y,
			z_extremes.second->position.z
		);

		vertices = std::vector<SimpleVertex>({ v1, v2, v3, v4, v5, v6, v7, v8 });
	}

	std::vector<SimpleTriangle> split_into_triangles() {
		return std::vector<SimpleTriangle>({
/*FRONT*/	SimpleTriangle({ vertices[0], vertices[1], vertices[2] }),
			SimpleTriangle({ vertices[2], vertices[3], vertices[0] }),
/*BOTTOM*/	SimpleTriangle({ vertices[7], vertices[4], vertices[0] }),
			SimpleTriangle({ vertices[0], vertices[3], vertices[7] }),
/*BACK*/	SimpleTriangle({ vertices[6], vertices[5], vertices[4] }),
			SimpleTriangle({ vertices[4], vertices[7], vertices[6] }),
/*LEFT*/	SimpleTriangle({ vertices[5], vertices[1], vertices[0] }),
			SimpleTriangle({ vertices[0], vertices[4], vertices[5] }),
/*RIGHT*/	SimpleTriangle({ vertices[2], vertices[6], vertices[7] }),
			SimpleTriangle({ vertices[7], vertices[3], vertices[2] }),
/*TOP*/		SimpleTriangle({ vertices[5], vertices[6], vertices[2] }),
			SimpleTriangle({ vertices[2], vertices[1], vertices[5] })
			});
	}
};

struct Box {
	std::vector<Vertex> vertices;

	Box() : vertices(std::vector<Vertex>(8)) { }
	Box(std::vector<Vertex> &verts) {
		if (verts.size() <= 8) {
			verts.reserve(8);
			vertices = verts;
		}
	}
};

using Cube = Box;

struct Line {
	FVector3 p1, p2;
};

struct Segment {
	FVector3 p1, p2;
};

struct Ray {
	FVector3 origin;
	FVector3 direction;
};

namespace Mechanics {
	class Force : public FVector3 {
	public:
		enum class Type {
			Impulse,
			Constant
		};

		std::string name = "";
		Type type;

		Force(FVector3 vector, std::string name = "",
			Type type = Type::Impulse) : FVector3(vector), name(name), type(type) { }

		bool operator==(const Force &force) const noexcept {
			return (name == force.name && type == force.type &&
				x == force.x && y == force.y && z == force.z);
		}
	};
};
