#pragma once
#include "../ObjectManager/ObjectManager.h"

namespace TargetSelector
{
	enum TargetType {
		LowestHealth,
	}; 
	
	GameObject* tryFindTarget(TargetType targetting_type, std::list<GameObject*> Objects);
}