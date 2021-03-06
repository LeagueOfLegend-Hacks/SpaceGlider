#include "LeagueFunctions.h"
#include <ctime>
float Functions::GameTime()
{
	return *(float*)(DEFINE_RVA(Offsets::Data::GameTime));
}

float Functions::GameTimeTick()
{
	return *(float*)(DEFINE_RVA(Offsets::Data::GameTime)) * 1000;
}

int Functions::GetPing() {
	typedef bool(__thiscall* fnGetPing)(void* netClient);
	fnGetPing pGetPing = (fnGetPing)(DEFINE_RVA(Offsets::Functions::GetPing));
	return pGetPing(*(void**)(DEFINE_RVA(Offsets::Data::NetClient)));
}

SpellData* Functions::GetBasicAttack(GameObject* unit)
{
	typedef SpellData** (__thiscall* fnGetBasicAttack)(GameObject* target, unsigned slot);
	auto ret = reinterpret_cast<fnGetBasicAttack>(DEFINE_RVA(Offsets::Functions::GetBasicAttack)
		)(unit, 0x41);
	if (ret) {
		return *ret;
	}
	return nullptr;
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

/*bool Functions::WorldToScreen(Vector3* in, Vector2* out)
{
	return reinterpret_cast<bool(__cdecl*)(Vector3*, Vector2*)>((__int32)GetModuleHandle(NULL) + static_cast<int>(Offsets::Functions::WorldToScreen))(in, out);
}
*/

bool Functions::IsWall(Vector3* position)
{
	typedef bool(__cdecl* fnIsNotWall)(Vector3* position, unsigned __int16 uk);
	return !((fnIsNotWall)(DEFINE_RVA(Offsets::Functions::IsNotWall)))(position, (unsigned __int16)0);
}

bool Functions::IsAlive(GameObject* Object)
{
	typedef bool(__thiscall* fnIsAlive)(GameObject* pObj);
	return ((fnIsAlive)(DEFINE_RVA(Offsets::Functions::IsAlive)))(Object) && Object->Health > 0.0f;
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
	if (MLP.enabled)
		return;
	DWORD HUDInputLogic = *(DWORD*)(*(DWORD*)DEFINE_RVA(Offsets::Data::HudInstance) + 0x24);
	Functions::IssueClick(HUDInputLogic, 0, 0, 1, screen_x, screen_y, static_cast<int>(orderType));
	Functions::IssueClick(HUDInputLogic, 1, 0, 1, screen_x, screen_y, static_cast<int>(orderType));
}
UINT PressKey(WORD scanCode)
{
	INPUT input[1] = { 0 };
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = 0;
	input[0].ki.wScan = scanCode;
	input[0].ki.dwFlags = KEYEVENTF_SCANCODE;

	UINT ret = SendInput(1, input, sizeof(INPUT));

	return ret;
}

UINT ReleaseKey(WORD scanCode)
{
	INPUT input[1] = { 0 };
	input[0].type = INPUT_KEYBOARD;
	input[0].ki.wVk = 0;
	input[0].ki.wScan = scanCode;
	input[0].ki.dwFlags = KEYEVENTF_SCANCODE | KEYEVENTF_KEYUP;

	UINT ret = SendInput(1, input, sizeof(INPUT));

	return ret;
}
void Functions::CastSpell(int Slot, int screen_x, int screen_y) {
		MLP.x = screen_x;
		MLP.y = screen_y;
		MLP.enabled = true;
		PressKey(0x10);
		ReleaseKey(0x10);
	action ReleaseMouse([=] {
		MLP.enabled = false;
		}, 8);
	DelayedAction.add(ReleaseMouse);
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