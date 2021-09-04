#include "OrbWalker.h"
#include <ctime>

float OrbWalker::LastAttackCommandT = 0;
float OrbWalker::LastMoveCommandT = 0;

Vector3 OrbWalker::GetMouseWorldPosition()
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

int OrbWalker::GetPing() {
	typedef bool(__thiscall* fnGetPing)(void* netClient);
	fnGetPing pGetPing = (fnGetPing)(DEFINE_RVA(Offsets::Functions::GetPing));
	return pGetPing(*(void**)(DEFINE_RVA(Offsets::Data::NetClient)));
}

void OrbWalker::IssueOrder(GameObject* ThisPtr, GameObjectOrder order, Vector3* position, GameObject* Target, bool IsAttack, bool IsMinion, bool Unknown) {
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

GameObject* OrbWalker::tryFindTarget(TargetType targetting_type) {
	auto pLocal = ObjectManager::GetLocalPlayer();
	std::list<GameObject*> Objects = ObjectManager::HeroList();
	GameObject* CurTarget = nullptr;
	for (auto pObject : Objects) {
		if (pObject != nullptr) {
			if (!pObject->IsHero())
				continue;
			if (pObject->Position.distance(pLocal->Position) < pLocal->AttackRange + pLocal->GetBoundingRadius()) {
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

bool OrbWalker::CanAttack() {
	return float(GetTickCount()) + GetPing() / 2.f >= LastAttackCommandT + ObjectManager::GetLocalPlayer()->GetAttackDelay() * 1000.f;
	//return float(GetTickCount()) + GetPing() / 2.f + 25.f >= LastAttackCommandT + ObjectManager::GetLocalPlayer()->GetAttackDelay() * 1000.f;
}

bool OrbWalker::CanMove(float extraWindup) {
	return GetTickCount() >= LastAttackCommandT + static_cast<int>(ObjectManager::GetLocalPlayer()->GetAttackCastDelay() * 1000.f) + GetPing()  + extraWindup || ObjectManager::GetLocalPlayer()->GetChampionName() == "Kalista";
	//return float(GetTickCount()) + GetPing() * 0.5f >= LastAttackCommandT + ObjectManager::GetLocalPlayer()->GetAttackCastDelay() * 1000.f + (GetPing() * 1.5f) + extraWindup;
}

void OrbWalker::OrbWalk(GameObject* target, float extraWindup) {
	DWORD HUDInputLogic = *(DWORD*)(*(DWORD*)DEFINE_RVA(Offsets::Data::HudInstance) + 0x24);
	typedef int(__thiscall* fnIssueClick)(int thisptr, int State, int IsAttack, int unknown2, int screen_x, int screen_y, char unknown3);
	fnIssueClick IssueClick = (fnIssueClick)(DEFINE_RVA(0x5AE550));
	if (CanAttack() && target != nullptr) {
		if (target->IsAlive()) {
			auto w2s = riot_render->WorldToScreen(target->ServerPosition);
			IssueClick(HUDInputLogic, 0, 1, 0, w2s.x, w2s.y, 0);
			IssueClick(HUDInputLogic, 1, 1, 0, w2s.x, w2s.y, 0);
			LastAttackCommandT = GetTickCount() + GetPing();
		}
	}
	if (CanMove(extraWindup) && LastMoveCommandT < GetTickCount())
	{
		auto w2s = riot_render->WorldToScreen(GetMouseWorldPosition());
		IssueClick(HUDInputLogic, 0, 0, 0, w2s.x, w2s.y, 0);
		IssueClick(HUDInputLogic, 1, 0, 0, w2s.x, w2s.y, 0);
		LastMoveCommandT = GetTickCount() + 50;
	}
}

void OrbWalker::OnDraw(LPDIRECT3DDEVICE9 Device)
{
	DWORD HUDInputLogic = *(DWORD*)(*(DWORD*)DEFINE_RVA(Offsets::Data::HudInstance) + 0x24);
	typedef int(__thiscall* fnIssueClick)(int thisptr, int State, int IsAttack, int unknown2, int screen_x, int screen_y, char unknown3);
	fnIssueClick IssueClick = (fnIssueClick)(DEFINE_RVA(0x5AE550));
	auto pLocal = ObjectManager::GetLocalPlayer();
	static clock_t lastAntiAfk = 0;
	render.draw_circle(pLocal->Position, pLocal->AttackRange + pLocal->GetBoundingRadius(), ImColor(0, 255, 0, 255));
	if (GetAsyncKeyState(VK_SPACE)) {
		OrbWalk(tryFindTarget(TargetType::LowestHealth),90.f);
	}
	/*
	else {
		if (lastAntiAfk == NULL || clock() - lastAntiAfk > 600) {
			lastAntiAfk = clock();
			auto w2s = riot_render->WorldToScreen(pLocal->ServerPosition);
			IssueClick(HUDInputLogic, 0, 0, 0, w2s.x, w2s.y, 0);
			IssueClick(HUDInputLogic, 1, 0, 0, w2s.x, w2s.y, 0);
		}
	}
	*/
}

void OrbWalker::OnProcessSpell(void* spellBook, SpellInfo* castInfo) {
	if (ObjectManager::GetLocalPlayer()->Index == castInfo->source_id) {
		/*
		switch (castInfo->Slot) {
		case kSpellSlot::SpellSlot_Unknown:
		case kSpellSlot::SpellSlot_BasicAttack1:
		case kSpellSlot::SpellSlot_BasicAttack2:
		case kSpellSlot::SpellSlot_SpecialAttack:
			LastAttackCommandT = float(GetTickCount64()) + GetPing() / 2;
			break;
		}
		*/
	}
}

void OrbWalker::Initalize() {
	LastMoveCommandT = 0;
	LastAttackCommandT = 0;
	riot_render = (D3DRenderer*)*(DWORD*)DEFINE_RVA(Offsets::Data::D3DRender);
	EventManager::AddEventHandler(EventManager::EventType::OnProcessSpell, OnProcessSpell);
	EventManager::AddEventHandler(EventManager::EventType::OnDraw, OnDraw);
}
