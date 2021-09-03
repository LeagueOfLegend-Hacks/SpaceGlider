#pragma once
#include "ObjectManager.h"
#include "D3DRenderer.h"
class OrbWalker {
private:
	D3DRenderer* riot_render = (D3DRenderer*)*(DWORD*)DEFINE_RVA(Offsets::Data::D3DRender);
	float LastAttackCommandT = 0;
	float LastMoveCommandT = 0;
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
	int GetPing() {
		typedef bool(__thiscall* fnGetPing)(void* netClient);
		fnGetPing pGetPing = (fnGetPing)(DEFINE_RVA(Offsets::Functions::GetPing));
		return pGetPing(*(void**)(DEFINE_RVA(Offsets::Data::NetClient)));
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
public:
	void OnTick() {
		if (GetAsyncKeyState(VK_SPACE)) {
			OrbWalk(this->tryFindTarget(TargetType::LowestHealth));
		}
	}
	void DoAAReset() {
		LastAttackCommandT = float(GetTickCount64()) + GetPing() / 2;
	}
};