#pragma once
#include <Windows.h>
#include <vector>
#include <VersionHelpers.h>
#include <Zydis/Zydis.h>
#include <windows.h>
#include <memory>

BOOL sys_VirtualProtect(LPVOID lpAddress, SIZE_T* dwSize, DWORD flNewProtect, PDWORD lpflOldProtect);

struct HookDetails {
	DWORD hookAddress;
	DWORD addressToHook;
};

struct HookEntries
{
	std::vector<HookDetails> hookDetails;
	DWORD addressToHookOldProtect;
	DWORD addressToHookMbiStart;
	DWORD addressToHookMbiEnd;
	DWORD addressToHookMbiSize;
	DWORD allocatedAddressStart;
	DWORD allocatedAddressEnd;
	DWORD allocatedAddressSize;
	DWORD addressToHookoffsetFromStart;
};

LONG WINAPI LeoHandler(EXCEPTION_POINTERS* pExceptionInfo);

class UltimateHooks {
public:
	template <class fnType>
	bool DEPAddHook(DWORD Address, DWORD hk_Address, fnType& OldAddress, size_t Size, PVOID& Allocation, uint8_t Offset) {
		DWORD NewOnprocessSpellAddr = VirtualAllocateRegion(Allocation, Address, Size);
		CopyRegion((DWORD)Allocation, Address, Size);
		FixRellocation(Address, (Address + Size), (DWORD)Allocation, Size, Offset);
		OldAddress = (fnType)(NewOnprocessSpellAddr);
		return addHook(Address, (DWORD)hk_Address, Offset);
	}
	bool deinit();
	DWORD RestoreRtlAddVectoredExceptionHandler();
	DWORD RestoreZwQueryInformationProcess();
	DWORD RestoreZwProtectVirtualMemory();
	bool addHook(DWORD address, DWORD hkAddress, size_t offset);
private:
	bool IsDoneInit = false;
	PVOID VEH_Handle = nullptr;

	DWORD UltimateHooks::VirtualAllocateRegion(PVOID& NewFunction, DWORD OrigAddress, size_t size);
	void UltimateHooks::CopyRegion(DWORD dest, DWORD source, size_t size);
	bool UltimateHooks::Hook(DWORD original_fun, DWORD hooked_fun, size_t offset);
	void UltimateHooks::FixRellocation(DWORD OldFnAddress, DWORD OldFnAddressEnd, DWORD NewFnAddress, size_t size, size_t _offset);
};
extern UltimateHooks UltHook;