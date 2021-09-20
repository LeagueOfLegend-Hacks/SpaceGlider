#include "TargetSelector.h"
#include "../Constants.h"
#include "../Utils/Helper.h"

bool TargetSelector::checkUnit(GameObject* unit)
{
	if (!Functions::IsAlive(unit) || !unit->IsVisible)
		return false;
	if (unit->IsAllyTo(ObjectManager::GetLocalPlayer()))
		return false;

	return true;
}

bool TargetSelector::IsValidUnit(GameObject* unit, float range, bool checkTeam, Vector3 from)
{
	if (unit == nullptr || !Functions::IsAlive(unit) || !unit->IsVisible)
		return false;
	if (checkTeam && unit->IsAllyTo(ObjectManager::GetLocalPlayer()))
		return false;
	if (unit->ChampionName == "WardCorpse")
		return false;
	if (!ObjectManager::GetLocalPlayer()->IsInRange(unit, range, true))
		false;
	return true;
}

GameObject* TargetSelector::GetTarget(float range, DamageType damageType, bool ignoreShield, Vector3 rangeCheckFrom)
{
	return GetTarget(Local, range, damageType, ignoreShield, rangeCheckFrom);
}

GameObject* TargetSelector::GetTarget(GameObject* champion, float range, DamageType damageType, bool ignoreShield, Vector3 rangeCheckFrom)
{
	TargetingMode mode = TargetingMode::LowHP;
	GameObject* first = nullptr;
	for (GameObject* next : filter(ObjectManager::HeroList(), [&](GameObject* enemy) { return Helper::isValidUnit(enemy) && enemy->IsInRange(Local, Local->AttackRange, true); }))
	{
				switch (mode)
				{
				case TargetingMode::LowHP:
					if (first == nullptr)
						first = next;
					if (first != nullptr && first->Health >= next->Health)
						first = next;
					break;
				case TargetingMode::MostAD:
					if (first == nullptr)
						first = next;
					if (first != nullptr && first->BaseAttackDamage + first->FlatPhysicalDamageMod > next->BaseAttackDamage + next->FlatPhysicalDamageMod)
						first = next;
					break;
				case TargetingMode::MostAP:
					if (first == nullptr)
						first = next;
					if (first != nullptr && first->BaseAbilityDamage + first->FlatMagicDamageMod > next->BaseAbilityDamage + next->FlatMagicDamageMod)
						first = next;
					break;
				case TargetingMode::Closest:
					if (first == nullptr)
						first = next;
					if (first != nullptr && (rangeCheckFrom.IsValid() ? rangeCheckFrom : champion->Position).DistanceSquared(first->Position) <= (rangeCheckFrom.IsValid() ? rangeCheckFrom : champion->Position).DistanceSquared(next->Position))
						first = next;
					break;
				case TargetingMode::NearMouse:
					if (first == nullptr)
						first = next;
					if (first != nullptr && first->Position.DistanceSquared(Functions::GetMouseWorldPosition()) <= next->Position.DistanceSquared(Functions::GetMouseWorldPosition()))
						first = next;
					break;
				};
	}
	return first;
}

