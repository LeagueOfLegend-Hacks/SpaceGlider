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
#include "../Plugins/Prediction/Health.h"
#include "../Utils/Helper.h"

namespace Orbwalking {
	inline std::list<GameObject*> MinionListAA;

	enum class OrbwalkingMode
	{
		LastHit,
		Mixed,
		LaneClear, 
		Combo, 
		Freeze, 
		CustomMode,
		None
	};

	enum TargetType {
		LowestHealth,
	};

	inline GameObject* _prevMinion = nullptr;
	inline int DelayOnFire = 0;
	inline unsigned int DelayOnFireId = 0;
	inline int BrainFarmInt = -100;
	inline float LaneClearWaitTimeMod = 2.f;
	inline OrbwalkingMode _mode = OrbwalkingMode::None;

	inline float LastAttackCommandT;
	inline float LastMoveCommandT;

	float GetMyProjectileSpeed();

	bool ShouldWait();

	bool CanAttack();
	bool CanMove(float extraWindup);
	GameObject* GetTarget();
	void Orbwalk(GameObject* target, float extraWindup = 0.0f);
	void OnUpdate();
	void OnDraw();
	void OnProcessSpell(void* spellBook, SpellInfo* castInfo);
	void OnDeleteObject(void* thisPtr, GameObject* obj);
	void Initialize();
	void OnLoad();
	void OnUnload();

	std::string AttackResets[];

}