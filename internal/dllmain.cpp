#define WIN32_LEAN_AND_MEAN
#include "Offsets.h"
#include "Decrypt.h"
#include "Console.h"
#include "Vector.h"
#include "UltimateHooks.h"
#include "ImRender.h"
#include <windows.h>
#include <detours.h>
#include <d3d9.h>
#include <ctime>
#include <string>
#include <unordered_map>
#include <functional>

enum class GameObjectOrder
{
	None = 0,
	HoldPosition,
	MoveTo,
	AttackUnit,
	AutoAttackPet,
	AutoAttack,
	MovePet,
	AttackTo,
	Stop = 10,
};
class D3DRenderer {
public:
	int GetWidth() {
		return *(int*)((DWORD)this + 0xC);
	}
	int GetHeight() {
		return *(int*)((DWORD)this + 0x10);
	}
	float* GetViewMatrix() {
		return (float*)((DWORD)this + 0x54);
	}
	float* GetProjectionMatrix() {
		return (float*)((DWORD)this + 0x94);
	}
	void MultiplyMatrices(float* out, float* a, float* b) {
		int size = 4 * 4;
		for (int i = 0; i < 4; i++) {
			for (int j = 0; j < 4; j++) {
				float sum = 0.f;
				for (int k = 0; k < 4; k++)
					sum = sum + a[i * 4 + k] * b[k * 4 + j];
				out[i * 4 + j] = sum;
			}
		}
	}
	Vector2 WorldToScreen(const Vector3& pos) {
		float viewProjMatrix[16];
		MultiplyMatrices(viewProjMatrix, GetViewMatrix(), GetProjectionMatrix());
		Vector2 screen = { (float)GetWidth(), (float)GetHeight() };
		static Vector4 clipCoords;
		clipCoords.x = pos.x * viewProjMatrix[0] + pos.y * viewProjMatrix[4] + pos.z * viewProjMatrix[8] + viewProjMatrix[12];
		clipCoords.y = pos.x * viewProjMatrix[1] + pos.y * viewProjMatrix[5] + pos.z * viewProjMatrix[9] + viewProjMatrix[13];
		clipCoords.z = pos.x * viewProjMatrix[2] + pos.y * viewProjMatrix[6] + pos.z * viewProjMatrix[10] + viewProjMatrix[14];
		clipCoords.w = pos.x * viewProjMatrix[3] + pos.y * viewProjMatrix[7] + pos.z * viewProjMatrix[11] + viewProjMatrix[15];
		if (clipCoords.w < 1.0f)
			clipCoords.w = 1.f;
		Vector3 M;
		M.x = clipCoords.x / clipCoords.w;
		M.y = clipCoords.y / clipCoords.w;
		M.z = clipCoords.z / clipCoords.w;
		return Vector2((screen.x / 2.f * M.x) + (M.x + screen.x / 2.f), -(screen.y / 2.f * M.y) + (M.y + screen.y / 2.f));
	}
}*riot_render;

typedef std::function<void()> callback;
struct action
{
	callback func;
	float    time;
	bool     executed;
	action(callback f, float t)
	{
		func = f;
		time = GetTickCount64()+t;
		executed = false;
	}
};
class delayAction
{
protected:
	std::vector<action> actionList;
public:
	void add(action a)
	{
		actionList.push_back(a);
	}
	void clear()
	{
		actionList.clear();
	}
	size_t size()
	{
		return actionList.size();
	}
	bool isReady()
	{
		for (auto& a : actionList) {
			if (!a.executed) {
				return false;
			}
		}
		return true;
	}
	void update(float time)
	{
		for (auto iter = actionList.begin(); iter != actionList.end(); ) {
			if ((*iter).executed == false && (*iter).time <= time) {
				(*iter).func();
				(*iter).executed = true;
				iter = actionList.erase(iter);
			}
			else ++iter;
		}
	}
}DelayedAction;
enum kSpellSlot
{
	SpellSlot_Unknown = -1,
	SpellSlot_Q,
	SpellSlot_W,
	SpellSlot_E,
	SpellSlot_R,
	SpellSlot_Summoner1,
	SpellSlot_Summoner2,
	SpellSlot_Item1,
	SpellSlot_Item2,
	SpellSlot_Item3,
	SpellSlot_Item4,
	SpellSlot_Item5,
	SpellSlot_Item6,
	SpellSlot_Trinket,
	SpellSlot_Recall,
	SpellSlot_SpecialAttack = 45,
	SpellSlot_BasicAttack1 = 64,
	SpellSlot_BasicAttack2 = 65,
};
class SpellInfo {
public:
	union {
		DEFINE_MEMBER_0(DWORD* base)
		DEFINE_MEMBER_N(kSpellSlot Slot, 0x4)
		DEFINE_MEMBER_N(float StartTime, 0x8)
		DEFINE_MEMBER_N(int SpellIndex, 0xC)
		DEFINE_MEMBER_N(unsigned int Level, 0x58)
		DEFINE_MEMBER_N(unsigned short source_id, 0x64)
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
struct GameObject {
public:
	union {
		DEFINE_MEMBER_0(DWORD* VTable)
		DEFINE_MEMBER_N(unsigned short Index, 0x20)
		DEFINE_MEMBER_N(int Team, 0x4c)
		DEFINE_MEMBER_N(unsigned int NetworkID, 0xCC)
		DEFINE_MEMBER_N(byte IsOnScreen, 0x1A8)
		DEFINE_MEMBER_N(ImRender::ImVec3 Position, 0x1d8)
		DEFINE_MEMBER_N(Vector3 ServerPosition, 0x1d8)
		DEFINE_MEMBER_N(bool IsTargetable, 0xD00)
		DEFINE_MEMBER_N(bool IsVisible, 0x23C)
		DEFINE_MEMBER_N(float Health, 0xD98);
		DEFINE_MEMBER_N(float MaxHealth, 0xDA8)
		DEFINE_MEMBER_N(float Armor, 0x1890)
		DEFINE_MEMBER_N(float BonusArmor, 0x18a0)
		DEFINE_MEMBER_N(float AttackRange, 0x12EC)
		DEFINE_MEMBER_N(float BaseAttackDamage, 0x17f0)
		DEFINE_MEMBER_N(float BonusAttackDamage, 0x121C)
		DEFINE_MEMBER_N(char* ChampionName, 0x2BB4);
	};
	float GameObject::GetBoundingRadius() {
		return reinterpret_cast<float(__thiscall*)(GameObject*)>(this->VTable[35])(this);
	}
	bool IsAlive() {
		typedef bool(__thiscall* fnIsAlive)(GameObject* pObj);
		return ((fnIsAlive)(DEFINE_RVA(Offsets::Functions::IsAlive)))(this);
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
		typedef bool(__cdecl* fnMinion)(GameObject* pObj);
		return ((fnMinion)(DEFINE_RVA(Offsets::Functions::IsMinion)))(this);
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
	float CalcDamage(GameObject* Target) {
		auto Damage = Target->BaseAttackDamage + Target->BonusAttackDamage;
		return Damage - (Target->Armor + Target->BonusArmor);
	}
};
PVOID NewOnProcessSpell, NewOnNewPath, NewOnCreateObject, NewOnDeleteObject;
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
	typedef bool(__cdecl* fnIsTroyEnt)(GameObject* pObj);
	typedef bool(__thiscall* fnGetPing)(GameObject* pObj);
}

LeagueDecrypt rito_nuke;
HMODULE g_module;
Console console;
UltimateHooks ulthook;
ImRender render;
clock_t lastAttack;
Vector3 GetMouseWorldPosition()
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
enum MouseSetting {
	DOWN,
	UP,
	HOLD,
};
template<typename T>
struct SEntityList {
	char pad[0x4];
	T** entities;
	size_t size;
	size_t max_size;
};
class ObjectManager {
private:


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
	static std::list<GameObject*> GetAllObjects() {
		std::list<GameObject*> Object;
		auto ObjectList = *reinterpret_cast<SEntityList<GameObject>**>(DEFINE_RVA(0x30CD584));
		for (size_t i = 0; i < ObjectList->size; i++)
		{
			auto Target = ObjectList->entities[i];
			if (Target->IsTargetable && Target->Health > 0.0f)
				Object.push_back(Target);
		}
		return Object;
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
	static std::list<GameObject*> GetAllMinions() {
		std::list<GameObject*> heroes;
		auto hero_list = *reinterpret_cast<SEntityList<GameObject>**>(DEFINE_RVA(0x30CD588));
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
	static DWORD* getUnderMouseObject()
	{
		return *reinterpret_cast<DWORD**>(DEFINE_RVA(0x180f208));
	}
};
namespace Functions {
	FuncTypes::Prototype_Reset Original_Reset;
	FuncTypes::Prototype_Present Original_Present;
	FuncTypes::fnOnProcessSpell OnProcessSpell;
	FuncTypes::fnOnNewPath OnNewPath;
	FuncTypes::fnCreateObject OnCreateObject;
	FuncTypes::fnDeleteObject OnDeleteObject;
	FuncTypes::orgGetPing GetPing;
	WNDPROC Original_WndProc;
}
int GetPing() {
	return Functions::GetPing(*(void**)(DEFINE_RVA(Offsets::Data::NetClient)));
}
float LastAttackCommandT = 0;
float LastMoveCommandT = 0;
bool CanAttack() {
	return float(GetTickCount64()) + GetPing() / 2.f >= LastAttackCommandT + ObjectManager::GetLocalPlayer()->GetAttackDelay() * 1000.f;
}
bool CanMove(float extraWindup) {
	return GetTickCount64() >= LastAttackCommandT + static_cast<int>(ObjectManager::GetLocalPlayer()->GetAttackCastDelay() * 1000) + GetPing() || ObjectManager::GetLocalPlayer()->GetChampionName() == "Kalista";
}
enum TargetType {
	LowestHealth,
};
GameObject* tryFindTarget(TargetType targetting_type) {
	auto pLocal = ObjectManager::GetLocalPlayer();
	std::list<GameObject*> Objects = ObjectManager::GetAllObjects();
	GameObject* CurTarget = nullptr;
	for (auto pObject : Objects) {
		if (pObject != nullptr) {
			if (pObject->ServerPosition.distance(pLocal->ServerPosition) < pLocal->AttackRange + pLocal->GetBoundingRadius()) {
				if (!pLocal->IsAllyTo(pObject) && pObject->IsTargetable && pObject->IsAlive()) {
					switch (targetting_type) {
					case TargetType::LowestHealth:
						if (CurTarget == nullptr)
							CurTarget = pObject;
						if (CurTarget != nullptr && CurTarget->Health > pObject->Health)
							CurTarget = pObject;
						break;
					}
				}
			}
		}
	}
	return CurTarget;
}
void IssueOrder(GameObject* ThisPtr, GameObjectOrder order, Vector3* position, GameObject* Target, bool IsAttack, bool IsMinion, bool Unknown) {
	DWORD HUDInputLogic = *(DWORD*)(*(DWORD*)DEFINE_RVA(0x183e1dc) + 0x24);
	DWORD HUDSpellLogic = *(DWORD*)(*(DWORD*)DEFINE_RVA(0x183e1dc) + 0x34);
	typedef int(__thiscall* fnIssueClick)(int thisptr, int State, int IsAttack, int unknown2, int screen_x, int screen_y, char unknown3);
	fnIssueClick IssueClick = (fnIssueClick)(DEFINE_RVA(0x5AE550));
	Vector2 w2s;
	switch (order) {
	case GameObjectOrder::MoveTo:
		w2s = riot_render->WorldToScreen(GetMouseWorldPosition());
		IssueClick(HUDInputLogic, 0, IsAttack, 0, w2s.x, w2s.y, 0);
		IssueClick(HUDInputLogic, 1, IsAttack, 0, w2s.x, w2s.y, 0);
		break;
	case GameObjectOrder::AttackUnit:
		if (Target != nullptr) {
			w2s = riot_render->WorldToScreen(Target->ServerPosition);
			IssueClick(HUDInputLogic, 0, IsAttack, 0, w2s.x, w2s.y, 0);
			IssueClick(HUDInputLogic, 1, IsAttack, 0, w2s.x, w2s.y, 0);
		}
		break;
	case GameObjectOrder::Stop:
		w2s = riot_render->WorldToScreen(ThisPtr->ServerPosition);
		IssueClick(HUDInputLogic, 0, IsAttack, 0, w2s.x, w2s.y, 0);
		IssueClick(HUDInputLogic, 1, IsAttack, 0, w2s.x, w2s.y, 0);
		break;
	}
}
void OrbWalk(GameObject* target, float extraWindup = 0.0f) {
	DWORD HUDInputLogic = *(DWORD*)(*(DWORD*)DEFINE_RVA(0x183e1dc) + 0x24);
	if (CanAttack() && LastAttackCommandT < GetTickCount64() && target != nullptr) {
		if (target->IsAlive()) {
			IssueOrder(ObjectManager::GetLocalPlayer(), GameObjectOrder::AttackTo, &target->ServerPosition, target, 1, 0, 1);
			LastAttackCommandT = float(GetTickCount64()) + GetPing() / 2;
		}
	}
	if (CanMove(extraWindup) && LastMoveCommandT < GetTickCount64())
	{
		IssueOrder(ObjectManager::GetLocalPlayer(), GameObjectOrder::MoveTo, &GetMouseWorldPosition(), nullptr, 0, 0, 1);
		LastMoveCommandT = GetTickCount64() + GetPing() + 50;
	}
}

void MainLoop() {
	if (GetAsyncKeyState(VK_SPACE)) {
		OrbWalk(tryFindTarget(TargetType::LowestHealth));
	}
}

HRESULT WINAPI Hooked_Present(LPDIRECT3DDEVICE9 Device, CONST RECT* pSrcRect, CONST RECT* pDestRect, HWND hDestWindow, CONST RGNDATA* pDirtyRegion) {
	render.Init(Device);
	render.begin_draw();
	MainLoop();
	console.Render();

	render.draw_text(ImVec2(5,5), "Space Glider", false, ImColor(255, 0, 0, 255));

	std::list<GameObject*> Objects = ObjectManager::GetAllObjects();
	for (auto obj : Objects) {
		if (obj->IsTurret()) {
			render.draw_circle(obj->Position, obj->AttackRange + obj->GetBoundingRadius(), ImColor(0, 255, 0, 255));
		}
	}

	auto herolist = ObjectManager::GetAllHeros();
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
	if (ObjectManager::GetLocalPlayer()->Index == CastInfo->source_id) {
		switch (CastInfo->Slot) {
		case kSpellSlot::SpellSlot_Unknown:
		case kSpellSlot::SpellSlot_BasicAttack1:
		case kSpellSlot::SpellSlot_BasicAttack2:
		case kSpellSlot::SpellSlot_SpecialAttack:
			LastAttackCommandT = float(GetTickCount64()) + GetPing() / 2;
			break;
		}
	}
	return Functions::OnProcessSpell(spellBook, CastInfo);
}
int hk_OnNewPath(GameObject* obj, ImRender::ImVec3* start, ImRender::ImVec3* end, ImRender::ImVec3* tail, int unk1, float* dashSpeed, unsigned dash, int unk3, char unk4, int unk5, int unk6, int unk7) {
	if (obj == nullptr)
		return Functions::OnNewPath(obj, start, end, tail, unk1, dashSpeed, dash, unk3, unk4, unk5, unk6, unk7);
	return Functions::OnNewPath(obj, start, end, tail, unk1, dashSpeed, dash, unk3, unk4, unk5, unk6, unk7);
}
int __fastcall hk_OnCreateObject(GameObject* obj, void* edx, unsigned int netId) {
	if (obj == nullptr)
		return Functions::OnCreateObject(obj, netId);
	return Functions::OnCreateObject(obj, netId);
}
int __fastcall hk_OnDeleteObject(void* thisPtr, void* edx, GameObject* obj) {
	if (obj == nullptr || thisPtr == nullptr)
		return Functions::OnDeleteObject(thisPtr, obj);
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
DWORD WINAPI MainThread(LPVOID param) {
#ifndef _DEBUG
	while (!(*(DWORD*)DEFINE_RVA(Offsets::Data::LocalPlayer)) && *(float*)(DEFINE_RVA(Offsets::Data::GameTime)) < 1)
		Sleep(1);
	Sleep(200);
#endif
	rito_nuke._RtlDispatchExceptionAddress = rito_nuke.FindRtlDispatchExceptionAddress();
	LeagueDecryptData ldd = rito_nuke.Decrypt(nullptr);

	Functions::GetPing = (FuncTypes::orgGetPing)(DEFINE_RVA(Offsets::Functions::GetPing));
	riot_render = (D3DRenderer*)*(DWORD*)DEFINE_RVA(0x310257c);
	ApplyHooks();

	while (!(GetAsyncKeyState(VK_END) & 1)) {
		DelayedAction.update(GetTickCount64());
	}

	RemoveHooks();

	render.Free();

	FreeLibraryAndExitThread(g_module, 0);
	return 1;
}
BOOL APIENTRY DllMain(HMODULE hModule,DWORD  ul_reason_for_call,LPVOID lpReserved)
{
	g_module = hModule;
	DisableThreadLibraryCalls(hModule);
	switch (ul_reason_for_call)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)MainThread, hModule, 0, nullptr);
		break;
	case DLL_THREAD_ATTACH:
		break;
	case DLL_THREAD_DETACH:
		break;
	case DLL_PROCESS_DETACH:
		break;
	}
	return TRUE;
}