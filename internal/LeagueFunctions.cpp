#include "LeagueFunctions.h"
FuncTypes::Prototype_Reset Functions::Original_Reset;
FuncTypes::Prototype_Present Functions::Original_Present;
FuncTypes::fnOnProcessSpell Functions::OnProcessSpell;
FuncTypes::fnOnNewPath Functions::OnNewPath;
FuncTypes::fnCreateObject Functions::OnCreateObject;
FuncTypes::fnDeleteObject Functions::OnDeleteObject;
FuncTypes::fnIssueClick Functions::IssueClick;

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

bool Functions::IsAlive(GameObject* Object)
{
	typedef bool(__thiscall* fnIsAlive)(GameObject* pObj);
	return ((fnIsAlive)(DEFINE_RVA(Offsets::Functions::IsAlive)))(Object);
}

bool Functions::IsHero(GameObject* Object)
{
	typedef bool(__cdecl* fnIsHero)(GameObject* pObj);
	return ((fnIsHero)(DEFINE_RVA(Offsets::Functions::IsHero)))(Object);
}

bool Functions::IsMissile(GameObject* Object)
{
	typedef bool(__cdecl* fnIsMissile)(GameObject* pObj);
	return ((fnIsMissile)(DEFINE_RVA(Offsets::Functions::IsMissile)))(Object);
}

bool Functions::IsMinion(GameObject* Object)
{
	typedef bool(__cdecl* fnMinion)(GameObject* pObj);
	return ((fnMinion)(DEFINE_RVA(Offsets::Functions::IsMinion)))(Object);
}

bool Functions::IsInhibitor(GameObject* Object)
{
	typedef bool(__cdecl* fnIsInhibitor)(GameObject* pObj);
	return ((fnIsInhibitor)(DEFINE_RVA(Offsets::Functions::IsInhib)))(Object);
}

bool Functions::IsBaron(GameObject* Object)
{
	typedef bool(__cdecl* fnIsBaron)(GameObject* pObj);
	return ((fnIsBaron)(DEFINE_RVA(Offsets::Functions::IsBaron)))(Object);
}

bool Functions::IsNexus(GameObject* Object)
{
	typedef bool(__cdecl* fnIsNexus)(GameObject* pObj);
	return ((fnIsNexus)(DEFINE_RVA(Offsets::Functions::IsNexus)))(Object);
}

bool Functions::IsTurret(GameObject* Object)
{
	typedef bool(__cdecl* fnIsTurret)(GameObject* pObj);
	return ((fnIsTurret)(DEFINE_RVA(Offsets::Functions::IsTurret)))(Object);
}

void Functions::IssueOrder(EOrderType orderType, int screen_x, int screen_y)
{
	DWORD HUDInputLogic = *(DWORD*)(*(DWORD*)DEFINE_RVA(Offsets::Data::HudInstance) + 0x24);
	Functions::IssueClick(HUDInputLogic, 0, static_cast<int>(orderType), 0, screen_x, screen_y, 0);
	Functions::IssueClick(HUDInputLogic, 1, static_cast<int>(orderType), 0, screen_x, screen_y, 0);
}

float Functions::GetAttackDelay(GameObject* Object)
{
	typedef float(__cdecl* fnGetAttackDelay)(GameObject* pObj);
	return ((fnGetAttackDelay)(DEFINE_RVA(Offsets::Functions::GetAttackDelay)))(Object);
}

float Functions::GetAttackCastDelay(GameObject* Object)
{
	typedef float(__cdecl* fnGetAttackCastDelay)(GameObject* pObj);
	return ((fnGetAttackCastDelay)(DEFINE_RVA(Offsets::Functions::GetAttackCastDelay)))(Object);
}