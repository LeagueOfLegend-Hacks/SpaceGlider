#pragma once
#include "../EventManager/EventManager.h"
#include "../ObjectManager/ObjectManager.h"
#include "Constants.h"
#include "../Evade/Evade.h"
#include "../Plugins/Prediction/Health.h"

#pragma region Plugin Includes
#include "../Plugins/Utilities/WaypointTracker.h"
#pragma endregion

namespace PluginLoader
{

	void LoadChampionModule()
	{
		switch (ObjectManager::GetLocalPlayer()->BaseCharacterData->SkinHash)
		{
		case Character::Vayne:
			console.Print("Vayne Loaded");
			break;
		}
	}

	void LoadPlugins()
	{
		//LoadChampionModule();

		WaypointTracker::Initialize();
		Orbwalking::Initialize();
		Evade::Core::Initalize();
		HealthPrediction::Initialize();

		if (EventManager::EventCallbacks->size() > 0)
			Constants::PluginsLoaded = true;
	}
}