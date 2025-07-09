#pragma once

#include <concepts>

template <typename T> concept arithmetic = std::is_arithmetic_v<T>;

#define GET_WITH_REASON(x) [[nodiscard(x)]]
#define GET [[nodiscard]]

#define NOMINMAX

#if defined(min)
	#undef min
#endif

#if defined(max)
	#undef max
#endif

#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }
