#pragma once
#define DEFINE_RVA(address) ((DWORD)GetModuleHandleA(NULL) + (DWORD)address)