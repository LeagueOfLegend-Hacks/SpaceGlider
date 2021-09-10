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
	enum TargetType {
		LowestHealth,
	};

	extern float LastAttackCommandT;
	extern float LastMoveCommandT;
	int GetGameTick();
	bool CanAttack();
	bool CanMove(float extraWindup);
	void OrbWalk(GameObject* target, float extraWindup = 0.0f);
	void OnDraw();
	void OnProcessSpell(void* spellBook, SpellInfo* castInfo);
	void Initialize();
	void OnLoad();
	void OnUnload();

	std::string AttackResets[];

}