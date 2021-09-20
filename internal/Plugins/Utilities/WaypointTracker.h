#pragma once
#include "../../EventManager/EventManager.h"
#include "../../Rendering/ImRender.h"
#include "../../ObjectManager/ObjectManager.h"

namespace WaypointTracker
{
	static bool validCheck(GameObject* unit)
	{
		if (unit->IsAllyTo(ObjectManager::GetLocalPlayer()))
			return false;
		if (!unit->IsVisible)
			return false;
		if (!Functions::IsAlive(unit))
			return false;
		return true;
	}

	void drawSpell(GameObject* unit, CSpellBase* spell, Vector2 pos)
	{
		ImColor color = spell->ready() ? ImColor(255, 255, 255) : ImColor(120, 120, 120);
		render.draw_image(spell->spellInfo()->spellData()->name, pos, Vector2(32, 32), color);
		if (!spell->ready()) {
			render.draw_text(Vector2(pos.x + 4, pos.y + 5), std::to_string((int)spell->remainingCooldown()).c_str(), false);
		}
	}

	void ShowWaypoints()
	{
		for (GameObject* hero : filter(ObjectManager::HeroList(), &validCheck))
		{
			Vector2 worldPos = riot_render->WorldToScreen(hero->Position);

			worldPos.x -= 70;

			worldPos.x += 35;
			drawSpell(hero, hero->SpellBook.spellId(0), worldPos);
			worldPos.x += 35;
			drawSpell(hero, hero->SpellBook.spellId(1), worldPos);
			worldPos.x += 35;
			drawSpell(hero, hero->SpellBook.spellId(2), worldPos);
			worldPos.x += 35;
			drawSpell(hero, hero->SpellBook.spellId(3), worldPos);
		}
		for (GameObject* hero : filter(ObjectManager::HeroList(), &validCheck))
		{
			if (!hero->GetAIManager()->Moving)
				continue;
			render.draw_image("singletarget", hero->GetAIManager()->NavEndPos, Vector2(36, 36), ImColor(255, 255, 255));
		}

		for (GameObject* hero : ObjectManager::HeroList())
		{
			if (!hero->IsVisible && hero->GetAIManager()->Moving && hero->IsEnemyTo(Local))
				render.draw_circle(hero->Position, ImColor(255, 255, 255));
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