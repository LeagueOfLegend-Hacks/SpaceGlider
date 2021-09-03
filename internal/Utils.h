#pragma once
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