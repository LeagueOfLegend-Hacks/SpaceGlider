#pragma once
#include "../EventManager/EventManager.h"
#include "../ObjectManager/ObjectManager.h"
#include "../Constants.h"

#pragma region Plugin Includes
#include "Utilities/WaypointTracker.h"
#pragma endregion

namespace PluginLoader
{

	void LoadChampionModule()
	{
		switch ( ObjectManager::GetLocalPlayer()->BaseCharacterData->SkinHash)
		{
		case Character::Vayne:
			console.Print("Vayne Loaded");
			break;
		}
	}

	void LoadPlugins()
	{
		LoadChampionModule();

		WaypointTracker::Initialize();
		OrbWalker::Initialize();
		Evade::Initalize();

		if (EventManager::EventCallbacks->size() > 0)
			Constants::PluginsLoaded = true;
	}
}