#pragma once
#include <Windows.h>
#include <d3d9.h>
#include "../Patchables/Structs.h"
#include "../DelayedAction/DelayedAction.h"
#include "../Console/Console.h"
namespace FuncTypes {
	typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
	typedef HRESULT(WINAPI* Prototype_Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
	typedef int(__thiscall* fnOnProcessSpell)(void* spellBook, SpellInfo* spellData);
	typedef int(__thiscall* fnCreateObject)(GameObject* obj, unsigned int NetworkID);
	typedef int(__thiscall* fnDeleteObject)(void* thisPtr, GameObject* pObject);
	typedef int(__cdecl* fnOnNewPath)(GameObject* obj, Vector3* start, Vector3* end, Vector3* tail, int unk1, float* dashSpeed, unsigned dash, int unk3, char unk4, int unk5, int unk6, int unk7);
	typedef int(__thiscall* fnOnFinishCast)(SpellInfo* ptr, GameObject* obj);
	typedef int(__thiscall* fnIssueClick)(int thisptr, int State, int IsAttack, int isAttackCommand, int screen_x, int screen_y, char unknown3);
	typedef int(__thiscall* fnIssueSpell)(int HudSpellLogic, int Slot, int always4, int State);
	typedef char* (__cdecl* fnIsVersion)(char* a1);
	typedef SpellData** (__thiscall* fnGetBasicAttack)(GameObject* target, unsigned slot);
}
namespace Functions {
	inline FuncTypes::Prototype_Reset Original_Reset;
	inline FuncTypes::Prototype_Present Original_Present;
	inline FuncTypes::fnOnProcessSpell OnProcessSpell;
	inline FuncTypes::fnOnNewPath OnNewPath;
	inline FuncTypes::fnCreateObject OnCreateObject;
	inline FuncTypes::fnDeleteObject OnDeleteObject;
	inline FuncTypes::fnOnFinishCast OnFinishCast;
	inline FuncTypes::fnIssueClick IssueClick;
	inline FuncTypes::fnIssueSpell IssueSpell;
	inline WNDPROC Original_WndProc;

	float GameTime();
	float GameTimeTick();
	int GetPing();
	SpellData* GetBasicAttack(GameObject* unit);
	Vector3 GetMouseWorldPosition();
	bool IsWall(Vector3* position);
	//bool WorldToScreen(Vector3* in, Vector2* out);
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
	void CastSpell(int Slot, int screen_x, int screen_y);
}