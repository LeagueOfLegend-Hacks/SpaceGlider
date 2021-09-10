#pragma once
#include "../EventManager/EventManager.h"
#include "../Rendering/ImRender.h"
#include"../ObjectManager/ObjectManager.h"

namespace WaypointTracker
{
	void ShowWaypoints()
	{
		for each (GameObject * hero in ObjectManager::HeroList())
		{
			if (hero->IsEnemyTo(ObjectManager::GetLocalPlayer()))
			{
				if (hero->IsVisible && Functions::IsAlive(hero))
					render.draw_line3D(hero->Position, hero->GetAIManager()->NavEndPos, ImColor(255, 0, 0, 255), 3.0f);
			}
		}
	}

	void OnUnload()
	{
		EventManager::RemoveEventHandler(EventManager::EventType::OnDraw, ShowWaypoints);
	}

	void Initialize()
	{
		EventManager::AddEventHandler(EventManager::EventType::OnDraw, ShowWaypoints);
		EventManager::AddEventHandler(EventManager::EventType::OnUnLoad, OnUnload);
	}
}