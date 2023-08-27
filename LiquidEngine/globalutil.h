#pragma once

#include <sstream>
#include <fstream>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <numbers>
#include <chrono>
#include <functional>
#include <filesystem>

#include <wrl.h>
#include <d3d12.h>

#include "globalstructs.h"

#define ACCEPT_BASE_AND_HEIRS_ONLY(T, base) T, typename = std::enable_if<std::is_base_of<base, T>::value>::type

#define GET_WITH_REASON(x) [[nodiscard(x)]]
#define GET [[nodiscard]]

#if defined(min)
	#undef min
#endif

#if defined(max)
	#undef max
#endif

namespace fs = std::filesystem;

using Microsoft::WRL::ComPtr;

enum class Unit {
	Meters,						// Distance
	Kilograms,					// Mass
	Newtons,					// Force
	MetersPerSecond,			// Velocity
	MetersPerSecondSquared,		// Acceleration
	Joules						// Kinetic Energy
};

static std::wstring string_to_wstring(const std::string& string, bool is_utf8 = true) {
	int len;
	int slength = (int)string.length() + 1;
	len = MultiByteToWideChar(is_utf8 ? CP_UTF8 : CP_ACP, 0, string.c_str(), slength, 0, 0);
	std::wstring buf;
	buf.resize(len);
	MultiByteToWideChar(is_utf8 ? CP_UTF8 : CP_ACP, 0, string.c_str(), slength,
		const_cast<wchar_t*>(buf.c_str()), len);
	return buf;
}

static std::string wstring_to_string(const std::wstring& wstring) {
	std::string str;
	str.resize(wstring.size());
	std::transform(wstring.begin(), wstring.end(), str.begin(),
		[](auto wc) {return static_cast<char>(wc); }
	);
	return str;
}

static std::wstring BSTR_to_wstring(BSTR bstr) {
	return std::wstring(bstr, SysStringLen(bstr));
}

static GET UVector2 get_window_size(HWND hwnd) {
	RECT rect;
	UVector2 size;
	if (GetWindowRect(hwnd, &rect)) {
		size.x = rect.right - rect.left;
		size.y = rect.bottom - rect.top;
	}
	return size;
}

static float degrees_to_theta(float degrees) {
	return degrees / 180.0f * (float)std::numbers::pi;
}

static void split(const std::string &s, char delim, std::vector<std::string> &elems) {
	std::stringstream ss(s);
	std::string item;
	while (std::getline(ss, item, delim)) {
		elems.push_back(item);
	}
}


static std::vector<std::string> split(const std::string &s, char delim) {
	std::vector<std::string> elems;
	split(s, delim, elems);
	return elems;
}

static void append_to_file(const std::string &message, const std::string &path = "out.log") {
	std::ofstream file;
	file.open(path, std::ios_base::app);

	file << message << std::endl;
}

static FVector3 cross(const FVector3 &a, const FVector3 &b) {
	FVector3 ret;  
	ret.x = a.y*b.z-a.z*b.y;
	ret.y = a.z*b.x-a.x*b.z;
	ret.z = a.x*b.y-a.y*b.x;
	return ret;
}

static float dot(const FVector3 &v1, const FVector3 &v2){
	return v1.x * v2.x + v1.y * v2.y;
}

static float normalize(float f) {
	return 1.0f / f;
}

static bool compf(float x, float y, float epsilon = 0.01f) {
	if(fabs(x - y) < epsilon)
		return true; //they are same
	return false; //they are not same
}

// trim from start (in place)
static void ltrim(std::string &s) {
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char ch) {
		return !std::isspace(ch);
		}));
}

// trim from end (in place)
static void rtrim(std::string &s) {
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char ch) {
		return !std::isspace(ch);
		}).base(), s.end());
}

// trim from both ends (in place)
static void trim(std::string &s) {
	ltrim(s);
	rtrim(s);
}

// trim from start (copying)
static std::string ltrim_copy(std::string s) {
	ltrim(s);
	return s;
}

// trim from end (copying)
static std::string rtrim_copy(std::string s) {
	rtrim(s);
	return s;
}

// trim from both ends (copying)
static std::string trim_copy(std::string s) {
	trim(s);
	return s;
}

static void remove_extra_spaces(std::string &s) {
	for (int i = 0; i < s.length(); i++) {
		if (s[i] == ' ' && s[i+1] == ' ')
			s.erase(i, 1);	
	}
}

static float distance(const FVector3 &p1, const FVector3 &p2) noexcept {
	return (float)sqrt(pow((p2.x-p1.x), 2) + pow((p2.y-p1.y), 2) + pow((p2.z-p1.z), 2));
}

static FVector2 UVector2_to_FVector2(const UVector2 &vector) noexcept {
	return FVector2((float)vector.x, (float)vector.y);
}

static FVector3 transform_vector(FVector3 vector, Transform transform) noexcept {
	XMVECTOR vec = XMVector3Transform(vector,
		XMMatrixTranspose(transform));

	FVector3 ret;

	ret.x = XMVectorGetX(vec);
	ret.y = XMVectorGetY(vec);
	ret.z = XMVectorGetZ(vec);

	return ret;
}

static std::vector<Vertex> tris_to_verts(const std::vector<Triangle> &tris) {
	std::vector<Vertex> ret;
	for (const Triangle &tri : tris) {
		ret.push_back(tri.first);
		ret.push_back(tri.second);
		ret.push_back(tri.third);
	}
	
	return ret;
}

static std::vector<SimpleVertex> tris_to_simple_verts(const std::vector<Triangle> &tris) noexcept {
	std::vector<SimpleVertex> ret;
	for (const Triangle &tri : tris) {
		ret.push_back((SimpleVertex)tri.first);
		ret.push_back((SimpleVertex)tri.second);
		ret.push_back((SimpleVertex)tri.third);
	}

	return ret;
}

static std::vector<SimpleVertex> simple_tris_to_simple_verts(const std::vector<SimpleTriangle> &tris) noexcept {
	std::vector<SimpleVertex> ret;
	for (const SimpleTriangle &tri : tris) {
		ret.push_back(tri.first.position);
		ret.push_back(tri.second.position);
		ret.push_back(tri.third.position);
	}

	return ret;
}

static std::vector<SimpleTriangle> tris_to_simple_tris(const std::vector<Triangle> &tris) {
	std::vector<SimpleTriangle> ret;
	for (const Triangle &tri : tris) {
		ret.push_back((SimpleTriangle)tri);
	}

	return ret;
}

static std::vector<SimpleVertex> verts_to_simple_verts(const std::vector<Vertex> &verts) {
	std::vector<SimpleVertex> ret;
	for (const Vertex &vert : verts) {
		ret.push_back(vert);
	}
	
	return ret;
}

static std::vector<Triangle> split_into_triangles(const std::vector<Vertex> &verts) noexcept {
	std::vector<Triangle> ret;

	for (size_t i = 0; i < verts.size(); i += 3){
		if (i + 2 < verts.size()) {
			ret.push_back({
				verts[i+0],
				verts[i+1],
				verts[i+2]
			});
		}
	}

	return ret;
}

static std::vector<SimpleTriangle> split_into_simple_triangles(const std::vector<SimpleVertex> &verts) noexcept {
	std::vector<SimpleTriangle> ret;

	for (size_t i = 0; i < verts.size(); i += 3){
		if (i + 2 < verts.size()) {
			ret.push_back({
				verts[i+0],
				verts[i+1],
				verts[i+2]
			});
		}
	}

	return ret;
}

static std::vector<Vertex> transform_vertices(const std::vector<Vertex> &verts, const Transform &transform) {
	std::vector<Vertex> ret = verts;

	for (Vertex &vertex : ret) {
		vertex.position = transform_vector(vertex.position, transform);
	}

	return ret;
}

static std::vector<SimpleVertex> transform_simple_vertices(const std::vector<SimpleVertex> &verts, const Transform &transform) {
	std::vector<SimpleVertex> ret = verts;

	for (SimpleVertex &vertex : ret) {
		vertex.position = transform_vector(vertex.position, transform);
	}

	return ret;
}

static std::vector<Triangle> transform_tris(const std::vector<Triangle> &tris, const Transform &transform) {
	std::vector<Triangle> ret = tris;

	for (Triangle &tri : ret) {
		tri.first.position = transform_vector(tri.first.position, transform);
		tri.second.position = transform_vector(tri.second.position, transform);
		tri.third.position = transform_vector(tri.third.position, transform);
	}

	return ret;
}

static std::vector<SimpleTriangle> transform_simple_tris(const std::vector<SimpleTriangle> &tris, const Transform &transform) {
	std::vector<SimpleTriangle> ret = tris;

	for (SimpleTriangle &tri : ret) {
		tri.first.position = transform_vector(tri.first.position, transform);
		tri.second.position = transform_vector(tri.second.position, transform);
		tri.third.position = transform_vector(tri.third.position, transform);
	}

	return ret;
}

static std::string get_parent_directory(const std::string &str, bool add_ending_slash = true) {
	std::string ret = str.substr(0, str.find_last_of("/\\"));
	if (add_ending_slash) {
		ret += "\\";
	}

	return ret;
}

static std::string format_time_point(const std::chrono::time_point<std::chrono::system_clock> &time_point) {
	return std::format("{0:%F %R %Z}", floor<std::chrono::milliseconds>(time_point));
}

//static bool operator==(const D3D12_RECT &lhs, const D3D12_RECT &rhs) noexcept {
//	return (lhs.left == rhs.left &&
//		lhs.right == rhs.right &&
//		lhs.top == rhs.top &&
//		lhs.bottom == rhs.bottom);
//}

static bool operator==(const D3D12_VERTEX_BUFFER_VIEW &lhs, const D3D12_VERTEX_BUFFER_VIEW &rhs) noexcept {
	return (lhs.BufferLocation == rhs.BufferLocation &&
		lhs.SizeInBytes == rhs.SizeInBytes &&
		lhs.StrideInBytes && rhs.StrideInBytes);
}

static bool operator==(const D3D12_STREAM_OUTPUT_DESC &lhs, const D3D12_STREAM_OUTPUT_DESC &rhs) noexcept {
	return (lhs.NumEntries == rhs.NumEntries &&
		lhs.NumStrides == rhs.NumStrides &&
		lhs.pBufferStrides == rhs.pBufferStrides &&
		lhs.pSODeclaration == rhs.pSODeclaration &&
		lhs.RasterizedStream == rhs.RasterizedStream);
}

static bool operator==(const D3D12_RASTERIZER_DESC &lhs, const D3D12_RASTERIZER_DESC &rhs) noexcept {
	return (lhs.AntialiasedLineEnable == rhs.AntialiasedLineEnable &&
		lhs.ConservativeRaster == rhs.ConservativeRaster &&
		lhs.CullMode == rhs.CullMode &&
		lhs.DepthBias == rhs.DepthBias &&
		lhs.DepthBiasClamp == rhs.DepthBiasClamp &&
		lhs.DepthClipEnable == rhs.DepthClipEnable &&
		lhs.FillMode == rhs.FillMode &&
		lhs.ForcedSampleCount == rhs.ForcedSampleCount &&
		lhs.FrontCounterClockwise == rhs.FrontCounterClockwise &&
		lhs.MultisampleEnable == rhs.MultisampleEnable &&
		lhs.SlopeScaledDepthBias == rhs.SlopeScaledDepthBias);
}

static bool operator==(const D3D12_ROOT_SIGNATURE_DESC &lhs, const D3D12_ROOT_SIGNATURE_DESC &rhs) noexcept {
	return (lhs.Flags == rhs.Flags &&
		lhs.NumParameters == rhs.NumParameters &&
		lhs.NumStaticSamplers == rhs.NumStaticSamplers &&
		lhs.pParameters == rhs.pParameters &&
		lhs.pStaticSamplers == rhs.pStaticSamplers
	);
}

static bool operator==(const D3D12_ROOT_DESCRIPTOR_TABLE &lhs, const D3D12_ROOT_DESCRIPTOR_TABLE &rhs) noexcept {
	return (lhs.NumDescriptorRanges == rhs.NumDescriptorRanges &&
		lhs.pDescriptorRanges == rhs.pDescriptorRanges);
}

static bool operator==(const D3D12_DESCRIPTOR_RANGE &lhs, const D3D12_DESCRIPTOR_RANGE &rhs) noexcept {
	return (lhs.BaseShaderRegister == rhs.BaseShaderRegister &&
		lhs.NumDescriptors == rhs.NumDescriptors &&
		lhs.OffsetInDescriptorsFromTableStart == rhs.OffsetInDescriptorsFromTableStart &&
		lhs.RangeType == rhs.RangeType &&
		lhs.RegisterSpace == rhs.RegisterSpace);
}

template <ACCEPT_DIGIT_ONLY(typename T)>
static T make_multiple(T x, T multiple) {
	if (multiple == 0)
		return x;

	T remainder = x % multiple;
	if (remainder == 0)
		return x;

	return x + multiple - remainder;
}

template <typename T>
static std::string address_to_string(const T* ptr) {
	const void* address = static_cast<const void*>(ptr);
	std::stringstream ss;
	ss << address;  
	return ss.str();
}

static XMFLOAT4X4 xmmatrix_to_4x4(const XMMATRIX &matrix) {
	XMFLOAT4X4 float4;
	XMStoreFloat4x4(&float4, matrix);
	return float4;
}

//template <typename T, typename U>
//static bool is_sorted(const std::vector<T> &vec, std::function<T(U obj)> get_obj = [](U obj) { return obj; }) {
//	for (int i = 1; i < vec.size(); i++) {
//		if (vec[i] <= vec[i-1]) return false;
//	}
//
//	return true;
//}
//
//template <typename T, typename U>
//static std::vector<T> sort(std::vector<T> vec, std::function<T(U obj)> get_obj = [](U obj) { return obj; }) {
//	while (!is_sorted(vec, get_obj)) {
//		for (int i = 1; i < vec.size(); i++) {
//			if (vec[i] <= vec[i-1]) {
//				vec.insert(vec.begin()+i-1, vec[i]);
//				vec.erase(vec.begin()+vec[i+1]);
//			}
//		}
//	}
//}

static constexpr FVector3 global_forward { 0.0f,  0.0f,  1.0f };
static constexpr FVector3 global_backward{ 0.0f,  0.0f, -1.0f };
static constexpr FVector3 global_left    {-1.0f,  0.0f,  0.0f };
static constexpr FVector3 global_right   { 1.0f,  0.0f,  0.0f };
static constexpr FVector3 global_up      { 0.0f,  1.0f,  0.0f };
static constexpr FVector3 global_down    { 0.0f, -1.0f,  0.0f };
