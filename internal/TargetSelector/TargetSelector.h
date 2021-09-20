#pragma once
#include "../ObjectManager/ObjectManager.h"

namespace TargetSelector
{
	bool checkUnit(GameObject* unit);
	bool IsValidUnit(GameObject* unit, float range = FLT_MAX, bool checkTeam = true, Vector3 from = Vector3());
	GameObject* GetTarget(float range, DamageType damageType, bool ignoreShield = true, Vector3 rangeCheckFrom = Vector3());
	GameObject* GetTarget(GameObject* champion, float range, DamageType damageType, bool ignoreShield = true, Vector3 rangeCheckFrom = Vector3(0, 0, 0));
}