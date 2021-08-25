#pragma once
#include <Windows.h>
#include <vector>


#define NT_SUCCESS(Status) (((NTSTATUS)(Status)) >= 0)

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
	DWORD RestoreRtlAddVectoredExceptionHandler();
	DWORD RestoreZwQueryInformationProcess();
	DWORD UltimateHooks::VirtualAllocateRegion(PVOID& NewFunction, DWORD OrigAddress, size_t size);
	void UltimateHooks::CopyRegion(DWORD dest, DWORD source, size_t size);
	void UltimateHooks::FixFuncRellocation(DWORD OldFnAddress, DWORD OldFnAddressEnd, DWORD NewFnAddress, size_t size);
	bool UltimateHooks::addHook(DWORD address, DWORD hkAddress, size_t offset);
private:
	bool IsDoneInit = false;
	PVOID VEH_Handle = nullptr;

	bool UltimateHooks::Hook(DWORD original_fun, DWORD hooked_fun, size_t offset);
	void UltimateHooks::FixRellocation(DWORD OldFnAddress, DWORD OldFnAddressEnd, DWORD NewFnAddress, size_t size, size_t _offset);
};