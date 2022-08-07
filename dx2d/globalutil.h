#pragma once

#include <sstream>
#include <fstream>
#include "globalstructs.h"

#define GET [[nodiscard]]

using uint = unsigned int;

#define COMPTR_RELEASE(x) { if ((x)) { (x).Reset(); } }

static GET Size2 get_window_size(HWND hwnd) {
	RECT rect;
	Size2 size;
	if (GetWindowRect(hwnd, &rect)) {
		size.x = rect.right - rect.left;
		size.y = rect.bottom - rect.top;
	}
	return size;
}

static float degrees_to_theta(float degrees) {
	return degrees / 180.0f * 3.141592654f;
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

static FColor color_to_fcolor(const Color &c) noexcept {
	return { (float)c.r, (float)c.g, (float)c.b, (float)c.a };
}

static void append_to_file(const std::string &path, const std::string &message) {
	std::ofstream file_out;
	file_out.open(path, std::ios_base::app);

	file_out << message.c_str() << std::endl;
}

static FVector3 cross(const FVector3 &a, const FVector3 &b) {
	FVector3 ret;  
	ret.x = a.y*b.z-a.z*b.y;
	ret.y = a.z*b.x-a.x*b.z;
	ret.z = a.x*b.y-a.y*b.x;
	return ret;
}

static float normalize(float f) {
	return 1.0f / f;
}

static bool compf(float x, float y, float epsilon = 0.01f) {
	if(fabs(x - y) < epsilon)
		return true; //they are same
	return false; //they are not same
}

//namespace Transformations {
//	static FVector3 rotate(FVector3 rx,
//		FVector3 ry, FVector3 rz,
//		FVector3 rotation, const FVector3 &point) noexcept {
//		// Rotation x = Target y & z
//		// Rotation y = Target x & z
//		// Rotation z = Up
//
//		float xpt = rx.matrix_multiplication(point);
//		float ypt = ry.matrix_multiplication(point);
//		float zpt = rz.matrix_multiplication(point);
//
//		FVector3 ret;
//		ret.x = xpt;
//		ret.y = ypt;
//		ret.z = zpt;
//
//		return ret;
//	}
//
//	static FVector3 rotate(FVector3 rotation, const FVector3 &point) noexcept {
//		FVector3 rx = {
//			cosf(0.0f)*cosf(rotation.y),
//			cosf(rotation.x)*sinf(rotation.y)*sinf(rotation.z)-sinf(rotation.x)*cosf(rotation.z),
//			cosf(rotation.x)*sinf(rotation.y)*cosf(rotation.z)+sinf(rotation.x)*sinf(rotation.z)
//		};
//		FVector3 ry = {
//			sinf(rotation.x)*cosf(rotation.y),
//			sinf(rotation.x)*sinf(rotation.y)*sinf(rotation.z)+cosf(rotation.x)*cosf(rotation.z),
//			sinf(rotation.x)*sinf(rotation.y)*cosf(rotation.z)-cosf(rotation.x)*sinf(rotation.z)
//		};
//		FVector3 rz = {
//			-sinf(rotation.y),
//			cosf(rotation.y)*sinf(rotation.z),
//			cosf(rotation.y)*cosf(rotation.z)
//		};
//
//		rotation.x = degrees_to_theta(rotation.x);
//		rotation.y = degrees_to_theta(rotation.y);
//		rotation.z = degrees_to_theta(rotation.z);
//
//		// Rotation x = Target y & z
//		// Rotation y = Target x & z
//		// Rotation z = Up
//
//		float xpt = rx.matrix_multiplication(point);
//		float ypt = ry.matrix_multiplication(point);
//		float zpt = rz.matrix_multiplication(point);
//
//		FVector3 ret;
//		ret.x = xpt;
//		ret.y = ypt;
//		ret.z = zpt;
//
//		return ret;
//	}
//
//	static FVector3 rotate_origin(FVector3 rx,
//		FVector3 ry, FVector3 rz,
//		FVector3 rotation, FVector3 &from_origin) noexcept {
//		// Rotation x = Target y & z
//		// Rotation y = Target x & z
//		// Rotation z = Up
//
//		float xpt = rx.matrix_multiplication(from_origin);
//		float ypt = ry.matrix_multiplication(from_origin);
//		float zpt = rz.matrix_multiplication(from_origin);
//
//		from_origin.x = xpt;
//		from_origin.y = ypt;
//		from_origin.z = zpt;
//
//		FVector3 ret;
//		ret.x = xpt;
//		ret.y = ypt;
//		ret.z = zpt;
//
//		return ret;
//	}
//
//	static FVector3 rotate_origin(FVector3 rotation, FVector3 &from_origin) noexcept {
//		rotation.x = degrees_to_theta(rotation.x);
//		rotation.y = degrees_to_theta(rotation.y);
//		rotation.z = degrees_to_theta(rotation.z);
//
//		FVector3 rx = {
//			cosf(0.0f)*cosf(rotation.y),
//			cosf(rotation.x)*sinf(rotation.y)*sinf(rotation.z)-sinf(rotation.x)*cosf(rotation.z),
//			cosf(rotation.x)*sinf(rotation.y)*cosf(rotation.z)+sinf(rotation.x)*sinf(rotation.z)
//		};
//		FVector3 ry = {
//			sinf(rotation.x)*cosf(rotation.y),
//			sinf(rotation.x)*sinf(rotation.y)*sinf(rotation.z)+cosf(rotation.x)*cosf(rotation.z),
//			sinf(rotation.x)*sinf(rotation.y)*cosf(rotation.z)-cosf(rotation.x)*sinf(rotation.z)
//		};
//		FVector3 rz = {
//			-sinf(rotation.y),
//			cosf(rotation.y)*sinf(rotation.z),
//			cosf(rotation.y)*cosf(rotation.z)
//		};
//
//		// Rotation x = Target y & z
//		// Rotation y = Target x & z
//		// Rotation z = Up
//
//		float xpt = rx.matrix_multiplication(from_origin);
//		float ypt = ry.matrix_multiplication(from_origin);
//		float zpt = rz.matrix_multiplication(from_origin);
//
//		from_origin.x = xpt;
//		from_origin.y = ypt;
//		from_origin.z = zpt;
//
//		FVector3 ret;
//		ret.x = xpt;
//		ret.y = ypt;
//		ret.z = zpt;
//
//		return ret;
//	}
//}

static constexpr FPosition3 global_forward { 0.0f,  0.0f,  1.0f};
static constexpr FPosition3 global_backward{ 0.0f,  0.0f, -1.0f};
static constexpr FPosition3 global_left    {-1.0f,  0.0f,  0.0f};
static constexpr FPosition3 global_right   { 1.0f,  0.0f,  0.0f};
static constexpr FPosition3 global_up      { 0.0f,  1.0f,  0.0f};
static constexpr FPosition3 global_down    { 0.0f, -1.0f,  0.0f};
