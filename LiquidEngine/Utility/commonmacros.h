#pragma once

#define ACCEPT_BASE_AND_HEIRS_ONLY(T, base) T, typename = std::enable_if<std::is_base_of<base, T>::value>::type

#define GET_WITH_REASON(x) [[nodiscard(x)]]
#define GET [[nodiscard]]

#if defined(min)
	#undef min
#endif

#if defined(max)
	#undef max
#endif

#define SAFE_RELEASE(p) { if ((p)) { (p)->Release(); (p) = nullptr; } }

#define ACCEPT_DIGIT_ONLY(T) T, typename = typename std::enable_if<std::is_arithmetic<T>::value, T>::type
