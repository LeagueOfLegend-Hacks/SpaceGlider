#include "LeagueFunctions.h"
FuncTypes::Prototype_Reset Functions::Original_Reset;
FuncTypes::Prototype_Present Functions::Original_Present;
FuncTypes::fnOnProcessSpell Functions::OnProcessSpell;
FuncTypes::fnOnNewPath Functions::OnNewPath;
FuncTypes::fnCreateObject Functions::OnCreateObject;
FuncTypes::fnDeleteObject Functions::OnDeleteObject;
WNDPROC Functions::Original_WndProc;
int Functions::GetPing() {
	typedef bool(__thiscall* fnGetPing)(void* netClient);
	fnGetPing pGetPing = (fnGetPing)(DEFINE_RVA(Offsets::Functions::GetPing));
	return pGetPing(*(void**)(DEFINE_RVA(Offsets::Data::NetClient)));
}
Vector3 Functions::GetMouseWorldPosition()
{
	DWORD MousePtr = DEFINE_RVA(Offsets::Data::HudInstance);
	auto aux1 = *(DWORD*)MousePtr;
	aux1 += 0x14;
	auto aux2 = *(DWORD*)aux1;
	aux2 += 0x1C;

	Vector3 temp;

	temp.x = *(float*)(aux2 + 0x0);
	temp.y = *(float*)(aux2 + 0x4);
	temp.z = *(float*)(aux2 + 0x8);

	return temp;
}