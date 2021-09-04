#include "OrbWalker.h"
#include <ctime>

float OrbWalker::LastAttackCommandT = 0;
float OrbWalker::LastMoveCommandT = 0;

GameObject* OrbWalker::tryFindTarget(TargetType targetting_type) {
	auto pLocal = ObjectManager::GetLocalPlayer();
	std::list<GameObject*> Objects = ObjectManager::HeroList();
	GameObject* CurTarget = nullptr;
	for (auto pObject : Objects) {
		if (pObject != nullptr) {
			if (!Functions::IsHero(pObject))
				continue;
			if (pObject->Position.distance(pLocal->Position) < pLocal->AttackRange + pLocal->GetBoundingRadius()) {
				if (!pLocal->IsAllyTo(pObject) && pObject->IsTargetable && Functions::IsAlive(pObject)) {
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
	return float(GetTickCount()) + Functions::GetPing() / 2.f >= LastAttackCommandT + Functions::GetAttackDelay(ObjectManager::GetLocalPlayer()) * 1000.f;
}

bool OrbWalker::CanMove(float extraWindup) {
	return GetTickCount() >= LastAttackCommandT + static_cast<int>(Functions::GetAttackCastDelay(ObjectManager::GetLocalPlayer()) * 1000.f) + Functions::GetPing()  + extraWindup || ObjectManager::GetLocalPlayer()->GetChampionName() == "Kalista";
}

void OrbWalker::OrbWalk(GameObject* target, float extraWindup) {
	DWORD HUDInputLogic = *(DWORD*)(*(DWORD*)DEFINE_RVA(Offsets::Data::HudInstance) + 0x24);
	typedef int(__thiscall* fnIssueClick)(int thisptr, int State, int IsAttack, int unknown2, int screen_x, int screen_y, char unknown3);
	fnIssueClick IssueClick = (fnIssueClick)(DEFINE_RVA(0x5AE550));
	if (CanAttack() && target != nullptr) {
		if (Functions::IsAlive(target)) {
			auto w2s = riot_render->WorldToScreen(target->ServerPosition);
			IssueClick(HUDInputLogic, 0, 1, 0, w2s.x, w2s.y, 0);
			IssueClick(HUDInputLogic, 1, 1, 0, w2s.x, w2s.y, 0);
			LastAttackCommandT = GetTickCount() + Functions::GetPing();
		}
	}
	if (CanMove(extraWindup) && LastMoveCommandT < GetTickCount())
	{
		auto w2s = riot_render->WorldToScreen(Functions::GetMouseWorldPosition());
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
