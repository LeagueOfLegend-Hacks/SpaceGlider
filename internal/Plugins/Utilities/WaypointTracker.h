#pragma once
#include "../EventManager/EventManager.h"
#include "../Rendering/ImRender.h"
#include"../ObjectManager/ObjectManager.h"

namespace WaypointTracker
{
	void drawSpell(GameObject* unit, CSpellBase* spell, Vector3 pos)
	{
		ImColor color = spell->ready() ? ImColor(255, 255, 255) : ImColor(120, 120, 120);
		if (spell->spellInfo()->spellData()->mana(spell->level) <= unit->Mana)
			color = ImColor(49, 97, 121);
		render.draw_image(spell->spellInfo()->spellData()->name, pos, Vector2(24, 24), color);
		if (spell->remainingCooldown() > 0.0f) {
			Vector2 worldPos = riot_render->WorldToScreen(pos);
			render.draw_text(Vector2(worldPos.x + 4, worldPos.y + 5), std::to_string(spell->remainingCooldown()).c_str(), false);
		}
	}

	void ShowWaypoints()
	{
		Vector3 pos = ObjectManager::GetLocalPlayer()->Position;
		pos.x -= 70;

		pos.x += 35;
		drawSpell(ObjectManager::GetLocalPlayer(), ObjectManager::GetLocalPlayer()->SpellBook.spellId(0), pos);
		pos.x += 35;
		drawSpell(ObjectManager::GetLocalPlayer(), ObjectManager::GetLocalPlayer()->SpellBook.spellId(1), pos);
		pos.x += 35;
		drawSpell(ObjectManager::GetLocalPlayer(), ObjectManager::GetLocalPlayer()->SpellBook.spellId(2), pos);
		pos.x += 35;
		drawSpell(ObjectManager::GetLocalPlayer(), ObjectManager::GetLocalPlayer()->SpellBook.spellId(3), pos);
		for (GameObject * hero : ObjectManager::HeroList())
		{
			if (!hero->IsVisible || !Functions::IsAlive(hero) || hero->IsAllyTo(ObjectManager::GetLocalPlayer()))
				continue;
			Vector3 pos = hero->Position;
			pos.x -= 70;

			pos.x += 35;
			drawSpell(hero, hero->SpellBook.spellId(0), pos);
			pos.x += 35;
			drawSpell(hero, hero->SpellBook.spellId(1), pos);
			pos.x += 35;
			drawSpell(hero, hero->SpellBook.spellId(2), pos);
			pos.x += 35;
			drawSpell(hero, hero->SpellBook.spellId(3), pos);
			if (!hero->GetAIManager()->Moving)
				continue;
			if (hero->IsVisible && Functions::IsAlive(hero)) {
				render.draw_image("singletarget", hero->GetAIManager()->NavEndPos, Vector2(36, 36), ImColor(255, 255, 255));
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