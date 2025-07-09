#pragma once

#include <sstream>
#include <fstream>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <numbers>
#include <chrono>
#include <functional>

#include "Vectors.h"
#include "SimpleShapes.h"
#include "ObjectStructs.h"
#include "commonmacros.h"

static std::string to_lower(std::string str) { // Must be copy
	std::transform(str.begin(), str.end(), str.begin(),
		[](unsigned char c){ return std::tolower(c); });
	
	return str;
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

static GET UVector2 get_client_size(HWND hwnd) {
	RECT rect;
	UVector2 size;
	if (GetClientRect(hwnd, &rect)) {
		size.x = rect.right - rect.left;
		size.y = rect.bottom - rect.top;
	}
	return size;
}

static GET UVector2 get_window_position(HWND hwnd) {
	RECT rect = { NULL };
	UVector2 pos;
	if (GetWindowRect(GetConsoleWindow(), &rect)) {
		pos.x = rect.left;
		pos.y = rect.top;
	}
	return pos;
}

static GET UVector2 get_client_position(HWND hwnd) {
	RECT rect = { NULL };
	UVector2 pos;
	if (GetClientRect(GetConsoleWindow(), &rect)) {
		pos.x = rect.left;
		pos.y = rect.top;
	}
	return pos;
}

static float degrees_to_radians(float degrees) {
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
	for (size_t i = 0; i < s.size(); i++) {
		if (s[i] == ' ' && s[i+1] == ' ')
			s.erase(i, 1u);
	}
}

static std::string get_parent_directory(const std::string &str, bool add_ending_slash = true) {
	std::string ret = str.substr(0, str.find_last_of("/\\"));
	if (add_ending_slash) {
		ret += "\\";
	}

	return ret;
}

template <arithmetic T>
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
