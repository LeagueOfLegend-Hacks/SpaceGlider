#define WIN32_LEAN_AND_MEAN
#include "Offsets.h"
#include "Decrypt.h"
#include "Utils.h"
#include "Console.h"
#include "UltimateHooks.h"
#include <windows.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <detours.h>
#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

struct SpellInfo {

};

extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

namespace FuncTypes {
	typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
	typedef HRESULT(WINAPI* Prototype_Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
	typedef int(__thiscall* fnOnProcessSpell)(void* spellBook, SpellInfo* spellData);
	typedef int(__thiscall* orgGetPing)(void* NetClient);
}

namespace Functions {
	FuncTypes::Prototype_Reset Original_Reset;
	FuncTypes::Prototype_Present Original_Present;
	FuncTypes::fnOnProcessSpell OnProcessSpell;
	FuncTypes::orgGetPing GetPing;
	WNDPROC Original_WndProc;
}

LeagueDecrypt rito_nuke;
HMODULE g_module;
Console console;
UltimateHooks ulthook;
PVOID NewOnProcessSpell;

int GetPing() {
	return Functions::GetPing(*(void**)(DEFINE_RVA(Offsets::Data::NetClient)));
}

HRESULT WINAPI Hooked_Present(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion)
{
	static bool init = true;
	if (init) {
		init = false;
		ImGui::CreateContext();
		ImGuiIO& io = ImGui::GetIO();
		ImGui_ImplWin32_Init(GetHwndProc());
		ImGui_ImplDX9_Init(Device);
	}
	ImGui_ImplDX9_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	console.Render();
	ImGui::Text("Ping: %i", GetPing());

	ImGui::EndFrame();
	ImGui::Render();

	ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
	return Functions::Original_Present(Device, pSrcRect, pDestRect, hDestWindow, pDirtyRegion);
}
HRESULT WINAPI Hooked_Reset(LPDIRECT3DDEVICE9 pDevice, D3DPRESENT_PARAMETERS* pPresentationParameters)
{
	ImGui_ImplDX9_InvalidateDeviceObjects();

	HRESULT result = Functions::Original_Reset(pDevice, pPresentationParameters);

	if (result >= 0)
		ImGui_ImplDX9_CreateDeviceObjects();

	return result;
}
LRESULT WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
		return true;

	return CallWindowProcA(Functions::Original_WndProc, hWnd, msg, wParam, lParam);
}
int __fastcall hk_OnProcessSpell(void* spellBook, void* edx, SpellInfo* CastInfo) {
	if (spellBook == nullptr || CastInfo == nullptr)
		return Functions::OnProcessSpell(spellBook, CastInfo);
	console.Print("OnProcessSpell Was Called.");
	return Functions::OnProcessSpell(spellBook, CastInfo);
}

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
#ifndef _DEBUG
	if (GetSystemDEPPolicy() && rito_nuke.IsMemoryDecrypted((PVOID)DEFINE_RVA(Offsets::Functions::OnProcessSpell))) {
		ulthook.DEPAddHook(DEFINE_RVA(Offsets::Functions::OnProcessSpell), (DWORD)hk_OnProcessSpell, Functions::OnProcessSpell, 0x60, NewOnProcessSpell, 1);
	}
#endif
}
void RemoveHooks() {
	SetWindowLongPtr(GetHwndProc(), GWLP_WNDPROC, (LONG_PTR)Functions::Original_WndProc);
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourDetach(&(PVOID&)Functions::Original_Present, Hooked_Present);
	DetourDetach(&(PVOID&)Functions::Original_Reset, Hooked_Reset);
	DetourTransactionCommit();
#ifndef _DEBUG
	if (GetSystemDEPPolicy())
		ulthook.deinit(); // this completely gets rid of all of our DEP hooks in one go.
#endif
}

DWORD WINAPI MainThread(LPVOID param) {
#ifndef _DEBUG
	while (!(*(DWORD*)DEFINE_RVA(Offsets::Data::LocalPlayer)) && *(float*)(DEFINE_RVA(Offsets::Data::GameTime)) < 1)
		Sleep(1);

	Sleep(5000);
#endif
	rito_nuke._RtlDispatchExceptionAddress = rito_nuke.FindRtlDispatchExceptionAddress();
	LeagueDecryptData ldd = rito_nuke.Decrypt(nullptr);

	Functions::GetPing = (FuncTypes::orgGetPing)(DEFINE_RVA(Offsets::Functions::GetPing));

	ApplyHooks();

	while (!(GetAsyncKeyState(VK_END) & 1))
		Sleep(1);

	RemoveHooks();

	ImGui_ImplDX9_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();

	FreeLibrary(g_module);
	return 1;
}
BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	g_module = hModule;
	DisableThreadLibraryCalls(hModule);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		_beginthreadex(nullptr, 0, (_beginthreadex_proc_type)MainThread, hModule, 0, nullptr);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}