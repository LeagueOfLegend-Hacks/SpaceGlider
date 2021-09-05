#pragma once
#include "ObjectManager.h"

namespace TargetSelector
{
	enum TargetType {
		LowestHealth,
	}; 
	
	GameObject* tryFindTarget(TargetType targetting_type);
}