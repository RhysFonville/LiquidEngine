#pragma once

#include <sstream>
#include <fstream>
#include <algorithm> 
#include <cctype>
#include <locale>
#include <numbers>

#include "globalstructs.h"

#define GET [[nodiscard]]

using uint = unsigned int;

#define COMPTR_RELEASE(x) { if ((x)) { (x).Reset(); } }

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
	for (int j = 0; j < s.length(); j++) {
		if (s[j] == ' ' && s[j+1] == ' ')
			s.erase(j, 1);	
	}
}

static float distance(const FVector3 &p1, const FVector3 &p2) noexcept {
	return (float)sqrt(pow((p2.x-p1.x), 2) + pow((p2.y-p1.y), 2) + pow((p2.z-p1.z), 2));
}

static FVector2 UVector2_to_FVector2(const UVector2 &vector) noexcept {
	return FVector2((float)vector.x, (float)vector.y);
}

static constexpr FVector3 global_forward { 0.0f,  0.0f,  1.0f};
static constexpr FVector3 global_backward{ 0.0f,  0.0f, -1.0f};
static constexpr FVector3 global_left    {-1.0f,  0.0f,  0.0f};
static constexpr FVector3 global_right   { 1.0f,  0.0f,  0.0f};
static constexpr FVector3 global_up      { 0.0f,  1.0f,  0.0f};
static constexpr FVector3 global_down    { 0.0f, -1.0f,  0.0f};
