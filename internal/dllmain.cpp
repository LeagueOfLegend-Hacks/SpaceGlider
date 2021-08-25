#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <imgui.h>
#include <imgui_impl_dx9.h>
#include <imgui_impl_win32.h>
#include <detours.h>

#include <d3d9.h>
#pragma comment(lib, "d3d9.lib")

typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
typedef HRESULT(WINAPI* Prototype_Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
extern LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

HWND GetHwndProc()
{
	HWND g_hWindow = GetTopWindow(NULL);
	DWORD currentPID = GetCurrentProcessId();
	do
	{
		char title[256];
		if ((GetWindowText(g_hWindow, title, 256) > 0) && (IsWindowVisible(g_hWindow)))
		{
			DWORD procId;
			GetWindowThreadProcessId(g_hWindow, &procId);

			if (procId == currentPID)
			{
				return g_hWindow;
			}
		}

		g_hWindow = GetNextWindow(g_hWindow, GW_HWNDNEXT);
	} while (g_hWindow);
}
static DWORD FindDevice(DWORD Len)
{
	DWORD dwObjBase = 0;

	dwObjBase = (DWORD)LoadLibrary("d3d9.dll");
	while (dwObjBase++ < dwObjBase + Len)
	{
		if ((*(WORD*)(dwObjBase + 0x00)) == 0x06C7
			&& (*(WORD*)(dwObjBase + 0x06)) == 0x8689
			&& (*(WORD*)(dwObjBase + 0x0C)) == 0x8689
			) {
			dwObjBase += 2; break;
		}
	}
	return(dwObjBase);
}
static DWORD GetDeviceAddress(int VTableIndex)
{
	PDWORD VTable;
	*(DWORD*)&VTable = *(DWORD*)FindDevice(0x128000);
	return VTable[VTableIndex];
}

namespace Functions {
	Prototype_Reset Original_Reset;
	Prototype_Present Original_Present;
	WNDPROC originalWindowProcedure;
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

	ImGui::Text("");

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

	return CallWindowProcA(Functions::originalWindowProcedure, hWnd, msg, wParam, lParam);
}

void ApplyHooks() {
	Functions::Original_Present = (Prototype_Present)GetDeviceAddress(17);
	Functions::Original_Reset = (Prototype_Reset)GetDeviceAddress(16);
	DetourRestoreAfterWith();
	DetourTransactionBegin();
	DetourUpdateThread(GetCurrentThread());
	DetourAttach(&(PVOID&)Functions::Original_Present, Hooked_Present);
	DetourAttach(&(PVOID&)Functions::Original_Reset, Hooked_Reset);
	DetourTransactionCommit();
	Functions::originalWindowProcedure = (WNDPROC)SetWindowLongPtr(GetHwndProc(), GWLP_WNDPROC, (LONG_PTR)WndProc);
}

DWORD WINAPI MainThread(LPVOID param) {
	Sleep(5000);
	ApplyHooks();
	while (true)
		Sleep(1);
	return 1;
}

BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	DisableThreadLibraryCalls(hModule);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, MainThread, hModule, 0, 0);
	case DLL_THREAD_ATTACH:
	case DLL_THREAD_DETACH:
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}