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