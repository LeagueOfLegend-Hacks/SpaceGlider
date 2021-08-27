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
#include <unordered_map>

class SpellInfo {
public:
	union {
		DEFINE_MEMBER_0(DWORD* base)
		DEFINE_MEMBER_N(int Slot, 0x4)
		DEFINE_MEMBER_N(float StartTime, 0x8)
		DEFINE_MEMBER_N(int SpellIndex, 0xC)
		DEFINE_MEMBER_N(unsigned int Level, 0x58)
		DEFINE_MEMBER_N(unsigned short SourceId, 0x64)
		DEFINE_MEMBER_N(unsigned int SourceNetworkID, 0x6C)
		DEFINE_MEMBER_N(ImRender::ImVec3 StartPosition, 0x7C)
		DEFINE_MEMBER_N(ImRender::ImVec3 EndPosition, 0x88)
		DEFINE_MEMBER_N(ImRender::ImVec3 CastPos, 0x94);
		DEFINE_MEMBER_N(bool HasTarget, 0xB4)
		DEFINE_MEMBER_N(unsigned short TargetId, 0xB8)
		DEFINE_MEMBER_N(float winduptime, 0x4B8)
		DEFINE_MEMBER_N(float CoolDown, 0x4CC)
		DEFINE_MEMBER_N(float ManaCost, 0x4E0)
		DEFINE_MEMBER_N(float EndTime, 0x7D0)
	};
};
std::unordered_map<int, struct SpellInfo> ActiveSpellMap;
struct GameObject {
public:
	union {
		DEFINE_MEMBER_0(DWORD* VTable)
		DEFINE_MEMBER_N(int Index, 0x20)
		DEFINE_MEMBER_N(int Team, 0x4c)
		DEFINE_MEMBER_N(unsigned int NetworkID, 0xCC)
		DEFINE_MEMBER_N(byte IsOnScreen, 0x1A8)
		DEFINE_MEMBER_N(ImRender::ImVec3 Position, 0x1D8)
		DEFINE_MEMBER_N(bool IsTargetable, 0xD00)
		DEFINE_MEMBER_N(bool IsVisible, 0x23C)
		DEFINE_MEMBER_N(float Health, 0xD98);
		DEFINE_MEMBER_N(float MaxHealth, 0xDA8)
		DEFINE_MEMBER_N(float AttackRange, 0x12EC)
		DEFINE_MEMBER_N(char* ChampionName, 0x2BB4);
	};
	float GameObject::GetBoundingRadius() {
		return reinterpret_cast<float(__thiscall*)(GameObject*)>(this->VTable[35])(this);
	}
	bool IsAlive() {
		typedef bool(__thiscall* fnIsAlive)(GameObject* pObj);
		return ((fnIsAlive)(DEFINE_RVA(Offsets::Functions::IsAlive)))(this) && this->Health > 0.0f;
	}
	bool IsHero() {
		typedef bool(__cdecl* fnIsHero)(GameObject* pObj);
		return ((fnIsHero)(DEFINE_RVA(Offsets::Functions::IsHero)))(this);
	}
	bool IsMissile() {
		typedef bool(__cdecl* fnIsMissile)(GameObject* pObj);
		return ((fnIsMissile)(DEFINE_RVA(Offsets::Functions::IsMissile)))(this);
	}
	bool IsMinion() {
		typedef bool(__cdecl* fnIsMinion)(GameObject* pObj);
		return ((fnIsMinion)(DEFINE_RVA(Offsets::Functions::IsMinion)))(this);
	}
	bool IsInhibitor() {
		typedef bool(__cdecl* fnIsInhibitor)(GameObject* pObj);
		return ((fnIsInhibitor)(DEFINE_RVA(Offsets::Functions::IsInhib)))(this);
	}
	bool IsBaron() {
		typedef bool(__cdecl* fnIsBaron)(GameObject* pObj);
		return ((fnIsBaron)(DEFINE_RVA(Offsets::Functions::IsBaron)))(this);
	}
	bool IsNexus() {
		typedef bool(__cdecl* fnIsNexus)(GameObject* pObj);
		return ((fnIsNexus)(DEFINE_RVA(Offsets::Functions::IsNexus)))(this);
	}
	bool IsTurret() {
		typedef bool(__cdecl* fnIsTurret)(GameObject* pObj);
		return ((fnIsTurret)(DEFINE_RVA(Offsets::Functions::IsTurret)))(this);
	}
	float GetAttackDelay() {
		typedef float(__cdecl* fnGetAttackDelay)(GameObject* pObj);
		return ((fnGetAttackDelay)(DEFINE_RVA(Offsets::Functions::GetAttackDelay)))(this);
	}
	float GetAttackCastDelay() {
		typedef float(__cdecl* fnGetAttackCastDelay)(GameObject* pObj);
		return ((fnGetAttackCastDelay)(DEFINE_RVA(Offsets::Functions::GetAttackCastDelay)))(this);
	}
	bool IsAllyTo(GameObject* Obj) {
		return Obj->Team == this->Team;
	}
	bool IsNeutral() {
		return this->Team == 300;
	}
	bool IsEnemyTo(GameObject* Obj) {
		if (this->IsNeutral() || Obj->IsNeutral())
			return false;
		return !IsAllyTo(Obj);
	}
	bool IsMonster() {
		return this->IsNeutral() && this->MaxHealth > 6.0f;
	}
	bool IsVisibleOnScreen() {
		return !(IsOnScreen % 2);
	}
	std::string GetChampionName() {
		return std::string(this->ChampionName);
	}
};
std::unordered_map<int, struct GameObject*> ActiveMissiles;

namespace FuncTypes {
	typedef HRESULT(WINAPI* Prototype_Present)(LPDIRECT3DDEVICE9, CONST RECT*, CONST RECT*, HWND, CONST RGNDATA*);
	typedef HRESULT(WINAPI* Prototype_Reset)(LPDIRECT3DDEVICE9, D3DPRESENT_PARAMETERS*);
	typedef int(__thiscall* fnOnProcessSpell)(void* spellBook, SpellInfo* spellData);
	typedef int(__thiscall* fnOnFinishCast)(SpellInfo* ptr, GameObject* obj);
	typedef int(__thiscall* fnCreateObject)(GameObject* obj, unsigned int NetworkID);
	typedef int(__thiscall* fnDeleteObject)(void* thisPtr, GameObject* pObject);
	typedef int(__thiscall* orgGetPing)(void* NetClient);
	typedef int(__cdecl* fnOnNewPath)(GameObject* obj, ImRender::ImVec3* start, ImRender::ImVec3* end, ImRender::ImVec3* tail, int unk1, float* dashSpeed, unsigned dash, int unk3, char unk4, int unk5, int unk6, int unk7);
	typedef void(__thiscall* tPrintChat)(DWORD ChatClient, const char* Message, int Color);
	typedef void(__cdecl* fnDrawCircle)(ImRender::ImVec3* position, float range, int* color, int a4, float a5, int a6, float alpha);
	typedef void(__thiscall* fnPrintChat)(DWORD ChatClient, const char* Message, int Color);
	typedef bool(__cdecl* WorldToScreen)(ImRender::ImVec3* vectorIn, ImRender::ImVec3* vectorOut);
	typedef bool(__cdecl* fnIsTroyEnt)(GameObject* pObj);
	typedef bool(__thiscall* fnGetPing)(GameObject* pObj);
}
struct renderer {
public:
	union {
		DEFINE_MEMBER_0(DWORD Base)
		DEFINE_MEMBER_N(float width, 0xC)
		DEFINE_MEMBER_N(float height, 0x10)
		DEFINE_MEMBER_N(D3DMATRIX* _matrix, 0x54)
	};
};

namespace Functions {
	FuncTypes::Prototype_Reset Original_Reset;
	FuncTypes::Prototype_Present Original_Present;
	FuncTypes::fnOnProcessSpell OnProcessSpell;
	FuncTypes::fnOnNewPath OnNewPath;
	FuncTypes::fnCreateObject OnCreateObject;
	FuncTypes::fnDeleteObject OnDeleteObject;
	FuncTypes::orgGetPing GetPing;
	FuncTypes::WorldToScreen WorldToScreen;
	FuncTypes::fnOnFinishCast OnFinishCast;
	WNDPROC Original_WndProc;
}

LeagueDecrypt rito_nuke;
HMODULE g_module;
Console console;
UltimateHooks ulthook;
ImRender render;
PVOID NewOnProcessSpell, NewOnNewPath, NewOnCreateObject, NewOnDeleteObject, NewOnFinishCast;
clock_t lastMove;
clock_t lastAttack;

ImRender::ImVec3 GetMouseWorldPosition()
{
	DWORD MousePtr = DEFINE_RVA(Offsets::Data::HudInstance);
	auto aux1 = *(DWORD*)MousePtr;
	aux1 += 0x14;
	auto aux2 = *(DWORD*)aux1;
	aux2 += 0x1C;

	ImRender::ImVec3 temp;

	temp.x = *(float*)(aux2 + 0x0);
	temp.y = *(float*)(aux2 + 0x4);
	temp.z = *(float*)(aux2 + 0x8);

	return temp;
}

int GetPing() {
	return Functions::GetPing(*(void**)(DEFINE_RVA(Offsets::Data::NetClient)));
}

class ObjectManager {
private:
	template<typename T>
	struct SEntityList {
		char pad[0x4];
		T** entities;
		size_t size;
		size_t max_size;
	};

public:
	static GameObject* GetFirstObject()
	{
		typedef GameObject* (__thiscall* fnGetFirst)(void*);
		return ((fnGetFirst)(DEFINE_RVA(Offsets::Functions::GetFirstObject)))(*(void**)(DEFINE_RVA(Offsets::Data::ObjectManager)));
	}
	static GameObject* GetNextObject(GameObject* object)
	{
		typedef GameObject* (__thiscall* fnGetNext)(void*, GameObject*);
		return ((fnGetNext)(DEFINE_RVA(Offsets::Functions::GetNextObject)))(*(void**)(DEFINE_RVA(Offsets::Data::ObjectManager)), object);
	}
	static std::unordered_map<unsigned int, GameObject*> GetAllObjects() {
		std::unordered_map<unsigned int, GameObject*> out;
		GameObject* Obj = ObjectManager::GetFirstObject();
		while (Obj) {
			out.insert({Obj->NetworkID, Obj});
			Obj = GetNextObject(Obj);
		}
		return out;
	}
	static GameObject* FindObjectByIndex(std::list<GameObject*> heroList, short casterIndex)
	{
		for (GameObject* a : heroList)
		{
			if (casterIndex == a->Index)
				return a;
		}
		return nullptr;
	}
	static std::list<GameObject*> GetAllHeros() {
		std::list<GameObject*> heroes;
		auto hero_list = *reinterpret_cast<SEntityList<GameObject>**>(DEFINE_RVA(Offsets::Data::ManagerTemplate_AIHero_));
		for (size_t i = 0; i < hero_list->size; i++)
		{
			auto hero = hero_list->entities[i];
			heroes.push_back(hero);
		}
		return heroes;
	}
	static GameObject* GetLocalPlayer() {
		return (GameObject*)*(DWORD*)DEFINE_RVA(Offsets::Data::LocalPlayer);
	}
};

struct Vector
{
	float X, Y, Z;

	Vector(void)
	{
	}

	Vector(const float x, const float y, const float z)
	{
		X = x;
		Y = y;
		Z = z;
	}

	Vector operator +(const Vector& A) const
	{
		return Vector(X + A.X, Y + A.Y, Z + A.Z);
	}

	Vector operator +(const float A) const
	{
		return Vector(X + A, Y + A, Z + A);
	}

	Vector operator *(const float A) const
	{
		return Vector(A * X, A * Y, A * Z);
	}

	Vector operator *(const Vector& A) const
	{
		return Vector(A.X * X, A.Y * Y, A.Z * Z);
	}

	Vector operator -(const float A) const
	{
		return Vector(A * X, A * Y, A * Z);
	}

	Vector operator -(const Vector& A) const
	{
		return Vector(A.X - X, A.Y - Y, A.Z - Z);
	}

	Vector operator /(const float A) const
	{
		return Vector(A / X, A / Y, A / Z);
	}

	Vector operator /(const Vector& A) const
	{
		return Vector(A.X / X, A.Y / Y, A.Z / Z);
	}

	float dot(const Vector& vec) const
	{
		return X * vec.X + Y * vec.Y + Z * vec.Z;
	}

	float lengthSquared()
	{
		return X * X + Y * Y + Z * Z;
	}

	float length()
	{
		return static_cast<float>(sqrt(lengthSquared()));
	}

	Vector perpendicularTo()
	{
		return Vector(Z, Y, -X);
	}

	Vector Normalize()
	{
		float length = this->length();
		if (length != 0)
		{
			float inv = 1.0f / length;
			X *= inv;
			Y *= inv;
			Z *= inv;
		}
		return Vector(X, Y, Z);
	}

	float DistTo(const Vector& A)
	{
		float out = sqrtf(powf(X - A.X, 2) + powf(Y - A.Y, 2) + powf(Z - A.Z, 2));
		return out < 0 ? out * -1 : out;
	}
};
float LastAttackCommandT = 0;
float LastMoveCommandT = 0;
void MoveCursorTo(float x, float y) {
	static float fScreenWidth = (float)::GetSystemMetrics(SM_CXSCREEN) - 1;
	static float fScreenHeight = (float)::GetSystemMetrics(SM_CYSCREEN) - 1;

	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE | MOUSEEVENTF_ABSOLUTE;
	input.mi.dx = (LONG)(x * (65535.0f / fScreenWidth));
	input.mi.dy = (LONG)(y * (65535.0f / fScreenHeight));

	SendInput(1, &input, sizeof(INPUT));
	SendInput(1, &input, sizeof(INPUT));
}
void PressRightClick()
{
	INPUT input = { 0 };
	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_RIGHTDOWN;
	SendInput(1, &input, sizeof(INPUT));
	Sleep(8);
	input.mi.dwFlags = MOUSEEVENTF_RIGHTUP;
	SendInput(1, &input, sizeof(INPUT));
}
bool CanAttack() {
	return float(GetTickCount64()) + 50 / 2.f + 25.f >= LastAttackCommandT + ObjectManager::GetLocalPlayer()->GetAttackDelay() * 1000.f;
}
bool CanMove(float extraWindup)
{
	return ObjectManager::GetLocalPlayer()->GetChampionName() == "Kalista" || float(GetTickCount64()) + 30 * 0.5f >= LastAttackCommandT + ObjectManager::GetLocalPlayer()->GetAttackCastDelay() * 1000.f + (GetPing() * 1.5f) + extraWindup;
}
void OrbWalk(GameObject* target, float extraWindup = 0.0f) {
	ImRender::ImVec3 previousPos;
	if (Functions::WorldToScreen(&GetMouseWorldPosition(), &previousPos)) {
		if (CanAttack() && LastAttackCommandT < GetTickCount64() && target != nullptr && target->IsAlive() && target->IsTargetable) {
			BlockInput(true);
			ImRender::ImVec3 TargetPos_W2S;
			if (Functions::WorldToScreen(&target->Position, &TargetPos_W2S)) {
				MoveCursorTo(TargetPos_W2S.x, TargetPos_W2S.y);
				Sleep(50);
				PressRightClick();
				Sleep(50);
				MoveCursorTo(previousPos.x, previousPos.y);
				LastAttackCommandT = float(GetTickCount64()) + rand() % 30 + GetPing();
			}
			BlockInput(false);
		}
		else if (CanMove(extraWindup) && LastMoveCommandT < GetTickCount64())
		{
			PressRightClick();
			LastMoveCommandT = GetTickCount64() + rand() % 30 + GetPing();
		}
	}
}

HRESULT WINAPI Hooked_Present(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion) {
	render.Init(Device);
	render.begin_draw();

	console.Render();

	render.draw_text(ImVec2(5,5), "Space Glider", false, ImColor(255, 0, 0, 255));
	auto herolist = ObjectManager::GetAllHeros();
	std::vector<int> Deletable;
	for (auto it = ActiveSpellMap.begin(); it != ActiveSpellMap.end(); it++) {
		if (it->second.StartTime > *(float*)(DEFINE_RVA(Offsets::Data::GameTime))) {
			ImRender::ImVec3 StartPos_W2S, EndPos_W2S;
			Functions::WorldToScreen(&it->second.StartPosition, &StartPos_W2S);
			Functions::WorldToScreen(&it->second.EndPosition, &EndPos_W2S);
			render.draw_line(StartPos_W2S, EndPos_W2S, ImColor(0.0f, 1.0f, 0.0f, 0.4f), 5.0);
		}
		else {
			Deletable.push_back(it->first);
		}
	}

	if (!Deletable.empty()) {
		for (auto dlt : Deletable)
			ActiveSpellMap.erase(dlt);
		Deletable.clear();
	}

	for (auto hero : herolist) {
		if (hero->IsAlive()) {
			if (hero->IsAllyTo(ObjectManager::GetLocalPlayer()))
				render.draw_circle(hero->Position, hero->AttackRange + hero->GetBoundingRadius(), ImColor(0, 255, 0, 255));
		}
	}

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
	ActiveSpellMap.insert(std::pair<int, struct SpellInfo>(CastInfo->SpellIndex, *CastInfo));
	return Functions::OnProcessSpell(spellBook, CastInfo);
}
int __fastcall hk_OnFinishCast(SpellInfo* castInfo, void* edx, GameObject* object) {
	if (!object || !castInfo)
		return Functions::OnFinishCast(castInfo, object);
	return Functions::OnFinishCast(castInfo, object);
}
int hk_OnNewPath(GameObject* obj, ImRender::ImVec3* start, ImRender::ImVec3* end, ImRender::ImVec3* tail, int unk1, float* dashSpeed, unsigned dash, int unk3, char unk4, int unk5, int unk6, int unk7) {
	if (obj == nullptr)
		return Functions::OnNewPath(obj, start, end, tail, unk1, dashSpeed, dash, unk3, unk4, unk5, unk6, unk7);

	return Functions::OnNewPath(obj, start, end, tail, unk1, dashSpeed, dash, unk3, unk4, unk5, unk6, unk7);
}
int __fastcall hk_OnCreateObject(GameObject* obj, void* edx, unsigned int netId) {
	if (obj == nullptr)
		return Functions::OnCreateObject(obj, netId);
	if (obj->IsMissile())
		ActiveMissiles.insert({ obj->Index, obj });
	return Functions::OnCreateObject(obj, netId);
}
int __fastcall hk_OnDeleteObject(void* thisPtr, void* edx, GameObject* obj) {
	if (obj == nullptr || thisPtr == nullptr)
		return Functions::OnDeleteObject(thisPtr, obj);
	if (obj->IsMissile())
		ActiveMissiles.erase(obj->Index);
	return Functions::OnDeleteObject(thisPtr, obj);
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
		ulthook.DEPAddHook(DEFINE_RVA(Offsets::Functions::OnCreateObject), (DWORD) hk_OnCreateObject, Functions::OnCreateObject, 0xAE, NewOnCreateObject, 3);
		ulthook.DEPAddHook(DEFINE_RVA(Offsets::Functions::OnDeleteObject), (DWORD) hk_OnDeleteObject, Functions::OnDeleteObject, 0x151, NewOnDeleteObject, 4);
		ulthook.DEPAddHook(DEFINE_RVA(Offsets::Functions::OnFinishCast), (DWORD)hk_OnFinishCast, Functions::OnFinishCast, 0x2A2, NewOnFinishCast, 5);
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

void MainLoop() {

	auto localPlayer = ObjectManager::GetLocalPlayer();
	GameObject* Object = ObjectManager::GetFirstObject();
	while (Object) {
		if (Object != nullptr) {
			Vector pos1(localPlayer->Position.x, localPlayer->Position.y, localPlayer->Position.z);
			Vector pos2(Object->Position.x, Object->Position.y, Object->Position.z);
			if (pos1.DistTo(pos2) < localPlayer->AttackRange + localPlayer->GetBoundingRadius()) {
				if (Object->IsMinion() || Object->IsHero() || Object->IsTurret() || Object->IsInhibitor() || Object->IsNexus()) {
					if (Object->IsEnemyTo(ObjectManager::GetLocalPlayer()) && Object->IsAlive() && Object->Health >= 3.0f) {
						if (GetAsyncKeyState(VK_SPACE)) {
							OrbWalk(Object);
						}
					}
				}
			}
		}
		if (GetAsyncKeyState(VK_SPACE))  {
			OrbWalk(nullptr);
		}
		Object = ObjectManager::GetNextObject(Object);
	}
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

	while (!(GetAsyncKeyState(VK_END) & 1)) {
		MainLoop();
	}

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