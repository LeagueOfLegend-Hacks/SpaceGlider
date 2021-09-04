#pragma once
#include "ObjectManager.h"
#include "EventManager.h"
#include "ImRender.h"
#include <unordered_map>
namespace Evade {
	extern std::unordered_map<float, SpellInfo> ActiveSpells;
	void OnDraw(LPDIRECT3DDEVICE9 Device);
	void OnProcessSpell(void* spellBook, SpellInfo* castInfo);
	void Initalize();
};