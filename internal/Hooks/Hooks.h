#pragma once

Console console;
ImRender render;

HRESULT WINAPI Hooked_Present(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion) {
	if (Device != nullptr) {

		render.Init(Device);
		render.begin_draw();
		console.Render();
		EventManager::Trigger(EventManager::EventType::OnDraw, Device);
		render.end_draw();
	}
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
auto Original_GetCursorPos = &GetCursorPos;
BOOL WINAPI hGetCursorPos(LPPOINT lpPoint)
{
	auto org = Original_GetCursorPos(lpPoint);
	if (MLP.enabled) {
		lpPoint->x = MLP.x;
		lpPoint->y = MLP.y;
	}
	return org;
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
	EventManager::Trigger(EventManager::EventType::OnProcessSpell, spellBook, CastInfo);
	return Functions::OnProcessSpell(spellBook, CastInfo);
}