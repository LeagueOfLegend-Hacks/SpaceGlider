#pragma once
#include "../ObjectManager/ObjectManager.h"
#include "../EventManager/EventManager.h"
#include "../Rendering/ImRender.h"
#include "../Console/Console.h"
#include "../Geometry/Geometry.h"
#include <unordered_map>
namespace Evade {
	extern std::unordered_map<float, SpellInfo> ActiveSpells;
	extern std::unordered_map<std::string, SpellDataEntry> SpellData;
	void OnDraw(LPDIRECT3DDEVICE9 Device);
	void OnProcessSpell(void* spellBook, SpellInfo* castInfo);
	void Initalize();
};