#define WIN32_LEAN_AND_MEAN
#include "Patchables/Offsets.h"
#include "Decrypt/Decrypt.h"
#include "Console/Console.h"
#include "Orbwalker/OrbWalker.h"
#include "Hooks/UltimateHooks.h"
#include "Rendering/ImRender.h"
#include <windows.h>
#include <detours.h>
#include <d3d9.h>
#include <ctime>
#include <string>
#include <list>
#include "Hooks/Hooks.h"
#include "Evade/Evade.h"
#include "Plugins/PluginLoader.h"
#include "ConfigManager/Config.h"
#include "Security/AntiDetection.h"

HMODULE g_module;
MouseLockedPos MLP;
uintptr_t initThreadHandle;
bool IsPatchDetected;
void ApplyHooks() {
	if (GetSystemDEPPolicy() != DEP_SYSTEM_POLICY_TYPE::DEPPolicyOptIn)
		SetProcessDEPPolicy(PROCESS_DEP_ENABLE);
	UltHook.RestoreZwProtectVirtualMemory();
	UltHook.RestoreRtlAddVectoredExceptionHandler();
	UltHook.RestoreZwQueryInformationProcess();
	Original_GetCursorPos = &GetCursorPos;
	Functions::Original_Present = (FuncTypes::Prototype_Present)GetDeviceAddress(17);
	Functions::Original_Reset = (FuncTypes::Prototype_Reset)GetDeviceAddress(16);
	Security::oModule32First = (Security::tModule32First)Module32First;
	Security::oModule32Next = (Security::tModule32Next)Module32Next;
	Security::oReadProcessMemory = (Security::tReadProcessMemory)ReadProcessMemory;
	/***********************************************************************************/
	// really need to get rid of ms detours. they're fast but i don't like them.
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	/***********************************************************************************/
	// dx9 functions
	DetourAttach(&(PVOID&)Functions::Original_Present, Hooked_Present);
	DetourAttach(&(PVOID&)Functions::Original_Reset, Hooked_Reset);
	/***********************************************************************************/
	// win32 functions
	DetourAttach(&(PVOID&)Original_GetCursorPos, hGetCursorPos);
	DetourAttach(&(PVOID&)Security::oModule32First, Security::hkModule32First);
	DetourAttach(&(PVOID&)Security::oModule32Next, Security::hkModule32Next);
	DetourAttach(&(PVOID&)Security::oReadProcessMemory, Security::hkReadProcessMemory);
	/***********************************************************************************/
	DetourTransactionCommit();
	/***********************************************************************************/
	Functions::Original_WndProc = (WNDPROC)SetWindowLongPtr(GetHwndProc(), GWLP_WNDPROC, (LONG_PTR)WndProc);
	if (GetSystemDEPPolicy() != DEP_SYSTEM_POLICY_TYPE::DEPPolicyAlwaysOff)
		UltHook.DEPAddHook(DEFINE_RVA(Offsets::Functions::OnProcessSpell), (DWORD)hk_OnProcessSpell, Functions::OnProcessSpell, 0x60, NewOnProcessSpell, 1);
}
void RemoveHooks() {
	SetWindowLongPtr(GetHwndProc(), GWLP_WNDPROC, (LONG_PTR)Functions::Original_WndProc);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)Original_GetCursorPos, hGetCursorPos);
	DetourDetach(&(PVOID&)Functions::Original_Present, Hooked_Present);
	DetourDetach(&(PVOID&)Functions::Original_Reset, Hooked_Reset);
	DetourDetach(&(PVOID&)Security::oModule32First, Security::hkModule32First);
	DetourDetach(&(PVOID&)Security::oModule32Next, Security::hkModule32Next);
	DetourDetach(&(PVOID&)Security::oReadProcessMemory, Security::hkReadProcessMemory);
	DetourTransactionCommit();
	if (GetSystemDEPPolicy())
		UltHook.deinit();
}
std::string GetGameVersion() {
	auto GameVersion = *(DWORD*)(TextDecryptor.FindSignature(nullptr, xorstr("8B 44 24 04 BA ? ? ? ? 2B D0")) + 0x5);
	return std::string(reinterpret_cast<char*>(GameVersion));
}
DWORD WINAPI MainThread(LPVOID param) {
	IsPatchDetected = GetGameVersion() != xorstr("Version 11.18.395.7538 [PUBLIC]");

	if (!IsPatchDetected) {
		while (!*(DWORD*)DEFINE_RVA(Offsets::Data::LocalPlayer) && *(float*)(DEFINE_RVA(Offsets::Data::GameTime)) < 1)
			Sleep(1);

		TextDecryptor._RtlDispatchExceptionAddress = TextDecryptor.FindRtlDispatchExceptionAddress();

		LeagueDecryptData ldd = TextDecryptor.Decrypt(nullptr);

		ApplyHooks();

		PluginLoader::LoadPlugins();

		EventManager::Trigger(EventManager::EventType::OnLoad);
		while (!(GetAsyncKeyState(VK_END) & 1)) {
			DelayedAction.update(GetTickCount64());
		}
	}
	else {
		TextDecryptor._RtlDispatchExceptionAddress = TextDecryptor.FindRtlDispatchExceptionAddress();
		LeagueDecryptData ldd = TextDecryptor.Decrypt(nullptr);
		MessageBoxA(nullptr, xorstr("A patch has been detected. please do a league dump now."), xorstr("PatchGuard"), 0);
	}

	return 1;
}
void OnExit() noexcept {
	EventManager::Trigger(EventManager::EventType::OnUnLoad);
	RemoveHooks();
	render.Free();
	//config.save(g_module);
	CloseHandle((HANDLE)initThreadHandle);
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	g_module = hModule;
	//config.load(hModule);
	DisableThreadLibraryCalls(hModule);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		std::atexit(OnExit);
		initThreadHandle = _beginthreadex(nullptr, 0, (_beginthreadex_proc_type)MainThread, hModule, 0, nullptr);
		FreeLibrary(hModule);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		OnExit();
		break;
	}
	return TRUE;
}
