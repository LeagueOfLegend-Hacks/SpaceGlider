#pragma once
#include "../ObjectManager/ObjectManager.h"

namespace TargetSelector
{
	enum TargetType {
		LowestHealth,
		Killable
	}; 
	
	GameObject* tryFindTarget(TargetType targetting_type, std::list<GameObject*> Objects);
}