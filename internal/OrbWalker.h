#pragma once
#include "ObjectManager.h"
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
};
class OrbWalker {
public:
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
	void DoOrbWalk(GameObject* target, float extraWindup = 0.0f) {
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
};