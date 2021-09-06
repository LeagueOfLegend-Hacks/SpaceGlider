#include "TargetSelector.h"

GameObject* TargetSelector::tryFindTarget(TargetType targetting_type)
{
	auto pLocal = ObjectManager::GetLocalPlayer();
	std::list<GameObject*> Objects = ObjectManager::HeroList();
	GameObject* CurTarget = nullptr;
	for (auto pObject : Objects) {
		if (pObject != nullptr) {
			if (pObject->Position.distance(pLocal->Position) < pLocal->AttackRange + pLocal->GetBoundingRadius()) {
				if (!pLocal->IsAllyTo(pObject) && pObject->IsTargetable && Functions::IsAlive(pObject)) {
					switch (targetting_type) {
					case TargetType::LowestHealth:
						if (CurTarget == nullptr)
							CurTarget = pObject;
						if (CurTarget != nullptr && CurTarget->Health > pObject->Health)
							CurTarget = pObject;
						break;
					}
				}
			}
		}
	}
	return CurTarget;
}
