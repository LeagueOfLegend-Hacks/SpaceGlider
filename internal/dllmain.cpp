#define WIN32_LEAN_AND_MEAN
#include "Offsets.h"
#include "Decrypt.h"
#include "Console.h"
#include "UltimateHooks.h"
#include "ImRender.h"
#include <windows.h>
#include <detours.h>
#include <d3d9.h>
#include <ctime>
#include <string>

struct SpellInfo {

};
struct Vector3 {
public:
	float x;
	float y;
	float z;
};
struct Vector2 {
	float x;
	float y;
};
struct GameObject {
public:
	union {
		DEFINE_MEMBER_0(DWORD* VTable)
		DEFINE_MEMBER_N(byte IsOnScreen, 0x1A8) // !(IsOnScreen % 2)
		DEFINE_MEMBER_N(ImRender::ImVec3 Position, 0x220)
	};
};

namespace FuncTypes {
	typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
	typedef HRESULT(WINAPI* Prototype_Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
	typedef int(__thiscall* fnOnProcessSpell)(void* spellBook, SpellInfo* spellData);
	typedef int(__thiscall* orgGetPing)(void* NetClient);
	typedef int(__cdecl* fnOnNewPath)(GameObject* obj, Vector3* start, Vector3* end, Vector3* tail, int unk1, float* dashSpeed, unsigned dash, int unk3, char unk4, int unk5, int unk6, int unk7);
	typedef void(__thiscall* tPrintChat)(DWORD ChatClient, const char* Message, int Color);
	typedef float(__cdecl* fnGetAttackCastDelay)(GameObject* pObj);
	typedef float(__cdecl* fnGetAttackDelay)(GameObject* pObj);
	typedef void(__cdecl* fnDrawCircle)(Vector3* position, float range, int* color, int a4, float a5, int a6, float alpha);
	typedef void(__thiscall* fnPrintChat)(DWORD ChatClient, const char* Message, int Color);
	typedef bool(__cdecl* WorldToScreen)(Vector3* vectorIn, Vector2* vectorOut);
	typedef bool(__cdecl* fnIsHero)(GameObject* pObj);
	typedef bool(__cdecl* fnIsMissile)(GameObject* pObj);
	typedef bool(__cdecl* fnIsMinion)(GameObject* pObj);
	typedef bool(__cdecl* fnIsTurret)(GameObject* pObj);
	typedef bool(__cdecl* fnIsInhibitor)(GameObject* pObj);
	typedef bool(__cdecl* fnIsTroyEnt)(GameObject* pObj);
	typedef bool(__cdecl* fnIsNexus)(GameObject* pObj);
	typedef float(__cdecl* fnGetAttackCastDelay)(GameObject* pObj);
	typedef float(__cdecl* fnGetAttackDelay)(GameObject* pObj);
	typedef bool(__thiscall* fnIsAlive)(GameObject* pObj);
	typedef bool(__thiscall* fnIsTargetable)(GameObject* pObj);
	typedef bool(__thiscall* fnGetPing)(GameObject* pObj);
}

namespace Functions {
	FuncTypes::Prototype_Reset Original_Reset;
	FuncTypes::Prototype_Present Original_Present;
	FuncTypes::fnOnProcessSpell OnProcessSpell;
	FuncTypes::fnOnNewPath OnNewPath;
	FuncTypes::orgGetPing GetPing;
	FuncTypes::WorldToScreen WorldToScreen;
	WNDPROC Original_WndProc;
}

LeagueDecrypt rito_nuke;
HMODULE g_module;
Console console;
UltimateHooks ulthook;
ImRender render;
PVOID NewOnProcessSpell, NewOnNewPath;

int GetPing() {
	return Functions::GetPing(*(void**)(DEFINE_RVA(Offsets::Data::NetClient)));
}

HRESULT WINAPI Hooked_Present(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion)
{
	render.Init(Device);
	render.begin_draw();

	console.Render();

	render.draw_text(ImVec2(5,5), "Space Glider", false, ImColor(255, 0, 0, 255));
	auto pLocal = (GameObject*)*(DWORD*)DEFINE_RVA(Offsets::Data::LocalPlayer);
	  
	render.draw_circle(pLocal->Position, 600, ImColor(255, 0, 0, 255));

	render.end_draw();
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
	console.Print("OnProcessSpell was called.");
	return Functions::OnProcessSpell(spellBook, CastInfo);
}
int hk_OnNewPath(GameObject* obj, Vector3* start, Vector3* end, Vector3* tail, int unk1, float* dashSpeed, unsigned dash, int unk3, char unk4, int unk5, int unk6, int unk7) {
	if (obj == nullptr)
		return Functions::OnNewPath(obj, start, end, tail, unk1, dashSpeed, dash, unk3, unk4, unk5, unk6, unk7);

	if (obj->IsOnScreen % 2)
		console.Print("offscreen");
	else
		console.Print("onscreen");

	console.Print("OnNewPath was called.");
	return Functions::OnNewPath(obj, start, end, tail, unk1, dashSpeed, dash, unk3, unk4, unk5, unk6, unk7);
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
		ulthook.DEPAddHook(DEFINE_RVA(Offsets::Functions::OnNewPath), (DWORD)hk_OnNewPath, Functions::OnNewPath, 0x28F, NewOnNewPath, 2);
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
	Functions::WorldToScreen = (FuncTypes::WorldToScreen)(DEFINE_RVA(0x971F30));

	ApplyHooks();

	while (!(GetAsyncKeyState(VK_END) & 1))
		Sleep(1);

	RemoveHooks();

	render.Free();

	FreeLibrary(g_module);
	_endthreadex(0);
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