#pragma once
#include "../ObjectManager/ObjectManager.h"
#include "../Rendering/D3DRenderer.h"
#include "../EventManager/EventManager.h"
#include "../Rendering/ImRender.h"
#include "../LeagueFunctions/LeagueFunctions.h"
#include "../TargetSelector/TargetSelector.h"
#include "../LeagueFunctions/LeagueFunctions.h"
#include "../Console/Console.h"
#include "../XorStr.h"
#include "../Evade/Evade.h"

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