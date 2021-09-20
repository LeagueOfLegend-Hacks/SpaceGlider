#include "Helper.h"
#include "../Patchables/Structs.h"
#include "../ObjectManager/ObjectManager.h"

bool Helper::isValidUnit(GameObject* unit, float range, bool checkTeam, Vector3 from)
{
	if (unit == NULL || !unit->IsVisible || !Functions::IsAlive(unit) || !unit->IsTargetable
		|| unit->IsInvulnearable)

	{
		return false;
	}

	if (checkTeam && unit->Team == Local->Team)
	{
		return false;
	}

	if (unit->GetChampionName() == "WardCorpse")
	{
		return false;
	}
	(from.IsValid() ? from : Local->Position).DistanceSquared(unit->Position) > range* range;
	return !(range < FLT_MAX) || (from.IsValid() ? from : Local->Position).DistanceSquared(unit->Position) > range * range;;
}