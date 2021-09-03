#pragma once
#include <Windows.h>
#include <d3d9.h>
#include "ImRender.h"
#include "Structs.h"
namespace FuncTypes {
	typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
	typedef HRESULT(WINAPI* Prototype_Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
	typedef int(__thiscall* fnOnProcessSpell)(void* spellBook, SpellInfo* spellData);
	typedef int(__thiscall* fnCreateObject)(GameObject* obj, unsigned int NetworkID);
	typedef int(__thiscall* fnDeleteObject)(void* thisPtr, GameObject* pObject);
	typedef int(__cdecl* fnOnNewPath)(GameObject* obj, Vector3* start, Vector3* end, Vector3* tail, int unk1, float* dashSpeed, unsigned dash, int unk3, char unk4, int unk5, int unk6, int unk7);
}
namespace Functions {
	FuncTypes::Prototype_Reset Original_Reset;
	FuncTypes::Prototype_Present Original_Present;
	FuncTypes::fnOnProcessSpell OnProcessSpell;
	FuncTypes::fnOnNewPath OnNewPath;
	FuncTypes::fnCreateObject OnCreateObject;
	FuncTypes::fnDeleteObject OnDeleteObject;
	WNDPROC Original_WndProc;
}