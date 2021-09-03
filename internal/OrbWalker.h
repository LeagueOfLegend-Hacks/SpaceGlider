#pragma once
#include "ObjectManager.h"
#include "D3DRenderer.h"
#include "EventManager.h"
#include "ImRender.h"

namespace OrbWalker {
	// move these to utils when possible
	Vector3 GetMouseWorldPosition();
	int GetPing();
	void IssueOrder(GameObject* ThisPtr, GameObjectOrder order, Vector3* position, GameObject* Target, bool IsAttack, bool IsMinion, bool Unknown);

	enum TargetType {
		LowestHealth,
	};

	extern float LastAttackCommandT;
	extern float LastMoveCommandT;
	GameObject* tryFindTarget(TargetType targetting_type);
	bool CanAttack();
	bool CanMove(float extraWindup);
	void OrbWalk(GameObject* target, float extraWindup = 0.0f);
	void OnTick();
	void OnProcessSpell(void* spellBook, SpellInfo* castInfo);
	void Initalize();
}