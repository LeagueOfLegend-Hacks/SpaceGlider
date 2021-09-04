#define WIN32_LEAN_AND_MEAN
#include "Offsets.h"
#include "Decrypt.h"
#include "Console.h"
#include "OrbWalker.h"
#include "UltimateHooks.h"
#include "ImRender.h"
#include <windows.h>
#include <detours.h>
#include <d3d9.h>
#include <ctime>
#include <string>
#include <list>
#include "Hooks.h"
#include "Evade.h"

LeagueDecrypt rito_nuke;
D3DRenderer* riot_render;
HMODULE g_module;
UltimateHooks ulthook;
PVOID NewOnProcessSpell, NewOnNewPath, NewOnCreateObject, NewOnDeleteObject;

void ApplyHooks() {
	if (GetSystemDEPPolicy())
		SetProcessDEPPolicy(PROCESS_DEP_ENABLE);
	ulthook.RestoreRtlAddVectoredExceptionHandler();
	ulthook.RestoreZwQueryInformationProcess();
	Functions::Original_Present = (FuncTypes::Prototype_Present)GetDeviceAddress(17);
	Functions::Original_Reset = (FuncTypes::Prototype_Reset)GetDeviceAddress(16);
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)Functions::Original_Present, Hooked_Present);
	DetourAttach(&(PVOID&)Functions::Original_Reset, Hooked_Reset);
	DetourTransactionCommit();
	Functions::Original_WndProc = (WNDPROC)SetWindowLongPtr(GetHwndProc(), GWLP_WNDPROC, (LONG_PTR)WndProc);
	if (GetSystemDEPPolicy()) {
		ulthook.DEPAddHook(DEFINE_RVA(Offsets::Functions::OnProcessSpell), (DWORD)hk_OnProcessSpell, Functions::OnProcessSpell, 0x60, NewOnProcessSpell, 1);
		//ulthook.DEPAddHook(DEFINE_RVA(Offsets::Functions::OnNewPath), (DWORD)hk_OnNewPath, Functions::OnNewPath, 0x28F, NewOnNewPath, 2);
		//ulthook.DEPAddHook(DEFINE_RVA(Offsets::Functions::OnCreateObject), (DWORD)hk_OnCreateObject, Functions::OnCreateObject, 0xAE, NewOnCreateObject, 3);
		//ulthook.DEPAddHook(DEFINE_RVA(Offsets::Functions::OnDeleteObject), (DWORD)hk_OnDeleteObject, Functions::OnDeleteObject, 0x151, NewOnDeleteObject, 4);
	}
}
void RemoveHooks() {
	SetWindowLongPtr(GetHwndProc(), GWLP_WNDPROC, (LONG_PTR)Functions::Original_WndProc);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)Functions::Original_Present, Hooked_Present);
	DetourDetach(&(PVOID&)Functions::Original_Reset, Hooked_Reset);
	DetourTransactionCommit();
	if (GetSystemDEPPolicy())
		ulthook.deinit();
}
DWORD WINAPI MainThread(LPVOID param) {
	while (!(*(DWORD*)DEFINE_RVA(Offsets::Data::LocalPlayer)) && *(float*)(DEFINE_RVA(Offsets::Data::GameTime)) < 1)
		Sleep(1);
	Sleep(200);
	rito_nuke._RtlDispatchExceptionAddress = rito_nuke.FindRtlDispatchExceptionAddress();
	LeagueDecryptData ldd = rito_nuke.Decrypt(nullptr);
	riot_render = (D3DRenderer*)*(DWORD*)DEFINE_RVA(Offsets::Data::D3DRender);
	ApplyHooks();
	OrbWalker::Initalize();
	Evade::Initalize();
	EventManager::Trigger(EventManager::EventType::OnLoad);
	while (!(GetAsyncKeyState(VK_END) & 1)) {
		EventManager::Trigger(EventManager::EventType::OnTick);
	}
	EventManager::Trigger(EventManager::EventType::OnUnLoad);
	RemoveHooks();

	render.Free();

	FreeLibraryAndExitThread(g_module, 0);
	return 1;
}
BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	g_module = hModule;
	DisableThreadLibraryCalls(hModule);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}