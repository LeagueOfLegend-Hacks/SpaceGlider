#pragma once
#ifdef _DEBUG
#error ONLY COMPILE IN RELEASE MODE. DO NOT COMPILE IN ANYTHING ELSE.
#endif
#ifdef _WIN64
#error YOU MUST COMPILE IN x86 or Win32, Win64 will not work.
#endif
#include <string>
#include <algorithm>
#include <Windows.h>
#include <vector>
#include <iostream>
#include <functional>
#include <list>
#include <iostream>
#include <ranges>

#define baseAddress (DWORD)GetModuleHandleA(NULL)
#define DEFINE_RVA(address) (baseAddress + (DWORD)address)
#define STR_MERGE_IMPL(x, y)                x##y
#define STR_MERGE(x,y)                        STR_MERGE_IMPL(x,y)
#define MAKE_PAD(size)                        BYTE STR_MERGE(pad_, __COUNTER__) [ size ]
#define DEFINE_MEMBER_0(x)                    x;
#define DEFINE_MEMBER_N(x,offset)            struct { MAKE_PAD((DWORD)offset); x; };

HWND GetHwndProc();
DWORD FindDevice(DWORD Len);
DWORD GetDeviceAddress(int VTableIndex);
bool IsLeagueInForeground();

template <class Functor>
class Not
{
public:
	Not(Functor& f) : func(f) {}

	template <typename ArgType>
	bool operator()(ArgType& arg) { return !func(arg); }

private:
	Functor& func;
};

template<typename T, typename B>
T filter(T list, B pred) {
	T ret;
	std::remove_copy_if(
		list.begin(),
		list.end(),
		std::back_insert_iterator<T>(ret),
		Not<B>(pred)
	);

	return ret;
}

template<typename T, typename B>
T filterVector(T vector, B pred) {
	T ret;
	std::remove_copy_if(
		vector.begin(),
		vector.end(),
		std::back_insert_iterator<T>(ret),
		Not<B>(pred)
	);

	return ret;
}

template<typename T>
struct min_max
{
	T min;
	T max;

	void emplace_value(T& val)
	{
		if (val > max)
			max = val;

		if (val < min)
			min = val;
	}
	float percentage(T& val) { return (float(val - min)) / float(max - min); }
};

std::string ToLower(std::string str);
std::wstring ToLower(std::wstring str);
bool StringContains(std::string strA, std::string strB, bool ignore_case = false);
bool StringContains(std::wstring strA, std::wstring strB, bool ignore_case = false);