#pragma once
#include <Windows.h>
#include <d3d9.h>
#include "../Patchables/Structs.h"
namespace FuncTypes {
	typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
	typedef HRESULT(WINAPI* Prototype_Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
	typedef int(__thiscall* fnOnProcessSpell)(void* spellBook, SpellInfo* spellData);
	typedef int(__thiscall* fnCreateObject)(GameObject* obj, unsigned int NetworkID);
	typedef int(__thiscall* fnDeleteObject)(void* thisPtr, GameObject* pObject);
	typedef int(__cdecl* fnOnNewPath)(GameObject* obj, Vector3* start, Vector3* end, Vector3* tail, int unk1, float* dashSpeed, unsigned dash, int unk3, char unk4, int unk5, int unk6, int unk7);
	typedef int(__thiscall* fnIssueClick)(int thisptr, int State, int IsAttack, int unknown2, int screen_x, int screen_y, char unknown3);
}
namespace Functions {
	extern FuncTypes::Prototype_Reset Original_Reset;
	extern FuncTypes::Prototype_Present Original_Present;
	extern FuncTypes::fnOnProcessSpell OnProcessSpell;
	extern FuncTypes::fnOnNewPath OnNewPath;
	extern FuncTypes::fnCreateObject OnCreateObject;
	extern FuncTypes::fnDeleteObject OnDeleteObject;
	extern FuncTypes::fnIssueClick IssueClick;
	extern WNDPROC Original_WndProc;

	int GetPing();
	Vector3 GetMouseWorldPosition();
	bool IsAlive(GameObject* Object);
	bool IsHero(GameObject* Object);
	bool IsMissile(GameObject* Object);
	bool IsMinion(GameObject* Object);
	bool IsInhibitor(GameObject* Object);
	bool IsBaron(GameObject* Object);
	bool IsNexus(GameObject* Object);
	bool IsTurret(GameObject* Object);
	float GetAttackDelay(GameObject* Object);
	float GetAttackCastDelay(GameObject* Object);
	void IssueOrder(EOrderType orderType, int screen_x, int screen_y);
}