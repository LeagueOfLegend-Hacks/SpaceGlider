#pragma once
#include "../ObjectManager/ObjectManager.h"
#include "../XorStr.h"
#include "../Console/Console.h"

class ModuleLoader
{
public:
	void LoadChampionModule();
};
extern ModuleLoader Loader;