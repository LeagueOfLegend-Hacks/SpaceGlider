#pragma once

Console console;
ImRender render;
OrbWalker orb;

HRESULT WINAPI Hooked_Present(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion) {
	EventManager::Trigger(EventManager::EventType::OnDraw, Device);
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
	EventManager::Trigger(EventManager::EventType::OnProcessSpell, spellBook, CastInfo);
	if (ObjectManager::GetLocalPlayer()->Index == CastInfo->source_id) {
		switch (CastInfo->Slot) {
		case kSpellSlot::SpellSlot_Unknown:
		case kSpellSlot::SpellSlot_BasicAttack1:
		case kSpellSlot::SpellSlot_BasicAttack2:
		case kSpellSlot::SpellSlot_SpecialAttack:
			orb.DoAAReset();
			break;
		}
	}
	return Functions::OnProcessSpell(spellBook, CastInfo);
}
int hk_OnNewPath(GameObject* obj, Vector3* start, Vector3* end, Vector3* tail, int unk1, float* dashSpeed, unsigned dash, int unk3, char unk4, int unk5, int unk6, int unk7) {
	if (obj == nullptr)
		return Functions::OnNewPath(obj, start, end, tail, unk1, dashSpeed, dash, unk3, unk4, unk5, unk6, unk7);
	EventManager::Trigger(EventManager::EventType::OnNewPath, obj, start, end, tail, dashSpeed, dash);
	return Functions::OnNewPath(obj, start, end, tail, unk1, dashSpeed, dash, unk3, unk4, unk5, unk6, unk7);
}
int __fastcall hk_OnCreateObject(GameObject* obj, void* edx, unsigned int netId) {
	if (obj == nullptr)
		return Functions::OnCreateObject(obj, netId);
	EventManager::Trigger(EventManager::EventType::OnCreateObject, obj, netId);
	return Functions::OnCreateObject(obj, netId);
}
int __fastcall hk_OnDeleteObject(void* thisPtr, void* edx, GameObject* obj) {
	if (obj == nullptr || thisPtr == nullptr)
		return Functions::OnDeleteObject(thisPtr, obj);
	EventManager::Trigger(EventManager::EventType::OnDeleteObject, obj);
	return Functions::OnDeleteObject(thisPtr, obj);
}