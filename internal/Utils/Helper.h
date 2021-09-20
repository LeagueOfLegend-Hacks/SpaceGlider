#pragma once
#include "../Patchables/Structs.h"

namespace Helper
{
	bool isValidUnit(GameObject* unit, float range = FLT_MAX, bool checkTeam = true, Vector3 from = Vector3());
};