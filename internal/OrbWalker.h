#pragma once
#include "ObjectManager.h"
#include "D3DRenderer.h"
#include "EventManager.h"
#include "ImRender.h"
#include "LeagueFunctions.h"

namespace OrbWalker {
	// move these to utils when possible
	void IssueOrder(GameObject* ThisPtr, GameObjectOrder order, Vector3* position, GameObject* Target, bool IsAttack, bool IsMinion, bool Unknown);

	enum TargetType {
		LowestHealth,
	};

	extern float LastAttackCommandT;
	extern float LastMoveCommandT;
	bool CanAttack();
	bool CanMove(float extraWindup);
	void OrbWalk(GameObject* target, float extraWindup = 0.0f);
	void OnDraw(LPDIRECT3DDEVICE9 Device);
	void OnProcessSpell(void* spellBook, SpellInfo* castInfo);
	void Initalize();

	std::string AttackResets[];

}