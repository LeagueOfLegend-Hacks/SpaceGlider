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
#ifdef _DEBUG
#error ONLY COMPILE IN RELEASE MODE. DO NOT COMPILE IN ANYTHING ELSE.
#endif
#ifdef _WIN64
#error YOU MUST COMPILE IN x86 or Win32, Win64 will not work.
#endif
HMODULE g_module;
MouseLockedPos MLP;


void ApplyHooks() {
	if (GetSystemDEPPolicy())
		SetProcessDEPPolicy(PROCESS_DEP_ENABLE);
	UltHook.RestoreRtlAddVectoredExceptionHandler();
	UltHook.RestoreZwQueryInformationProcess();
	Original_GetCursorPos = &GetCursorPos;
	Functions::Original_Present = (FuncTypes::Prototype_Present)GetDeviceAddress(17);
	Functions::Original_Reset = (FuncTypes::Prototype_Reset)GetDeviceAddress(16);
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)Original_GetCursorPos, hGetCursorPos);
	DetourAttach(&(PVOID&)Functions::Original_Present, Hooked_Present);
	DetourAttach(&(PVOID&)Functions::Original_Reset, Hooked_Reset);
	DetourTransactionCommit();
	Functions::Original_WndProc = (WNDPROC)SetWindowLongPtr(GetHwndProc(), GWLP_WNDPROC, (LONG_PTR)WndProc);
	if (GetSystemDEPPolicy()) {
		UltHook.DEPAddHook(DEFINE_RVA(Offsets::Functions::OnProcessSpell), (DWORD)hk_OnProcessSpell, Functions::OnProcessSpell, 0x60, NewOnProcessSpell, 1);
	}
}
void RemoveHooks() {
	SetWindowLongPtr(GetHwndProc(), GWLP_WNDPROC, (LONG_PTR)Functions::Original_WndProc);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)Original_GetCursorPos, hGetCursorPos);
	DetourDetach(&(PVOID&)Functions::Original_Present, Hooked_Present);
	DetourDetach(&(PVOID&)Functions::Original_Reset, Hooked_Reset);
	DetourTransactionCommit();
	if (GetSystemDEPPolicy())
		UltHook.deinit();
}
DWORD WINAPI MainThread(LPVOID param) {
	//config.load(g_module);
	while (!(*(DWORD*)DEFINE_RVA(Offsets::Data::LocalPlayer)) && *(float*)(DEFINE_RVA(Offsets::Data::GameTime)) < 1)
		Sleep(1);
	Sleep(200);
	TextDecryptor._RtlDispatchExceptionAddress = TextDecryptor.FindRtlDispatchExceptionAddress();
	LeagueDecryptData ldd = TextDecryptor.Decrypt(nullptr);
	ApplyHooks();

	PluginLoader::LoadPlugins();
	OrbWalker::Initialize();
	Evade::Initalize();

	EventManager::Trigger(EventManager::EventType::OnLoad);
	while (!(GetAsyncKeyState(VK_END) & 1)) {
		DelayedAction.update(GetTickCount64());
	}
	EventManager::Trigger(EventManager::EventType::OnUnLoad);
	RemoveHooks();

	render.Free();
	//config.save(g_module);
	return 1;
}
uintptr_t initThreadHandle;
void OnExit() noexcept
{
	CloseHandle((HANDLE)initThreadHandle);
}
BOOL APIENTRY DllMain(HMODULE hModule, DWORD  ul_reason_for_call, LPVOID lpReserved) {
	g_module = hModule;
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
