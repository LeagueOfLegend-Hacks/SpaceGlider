#include "Evade.h"

std::unordered_map<float, SpellInfo> Evade::ActiveSpells;

void Evade::OnDraw(LPDIRECT3DDEVICE9 Device) {
	std::unordered_map<float, SpellInfo>::iterator it;
	// also need to iterate through missiles and filter based off spell names and such.
	for (it = ActiveSpells.begin(); it != ActiveSpells.end(); it++) {
		if (it->second.StartTime > *(float*)(DEFINE_RVA(Offsets::Data::GameTime))) {
			Vector2 StartPos_W2S, EndPos_W2S;
			StartPos_W2S = riot_render->WorldToScreen(it->second.StartPosition);
			EndPos_W2S = riot_render->WorldToScreen(it->second.EndPosition);

			auto Caster = ObjectManager::FindObjectByIndex(ObjectManager::HeroList(), it->second.source_id);

			//auto SpellWidth = Caster->GetSpellSlotByID(it->second.SpellSlot)->GetSpellInfo()->GetSpellData()->GetSpellWidth();
			auto SpellWidth = 10.0f;

			ImColor Color = ImColor(1.0f, 1.0f, 1.0f, 0.4f);
			if (Caster->IsEnemyTo(ObjectManager::GetLocalPlayer()))
				Color = ImColor(1.0f, 0.0f, 0.0f, 0.4f);
			else
				Color = ImColor(0.0f, 1.0f, 0.0f, 0.4f);

			render.draw_line(StartPos_W2S, EndPos_W2S, Color, SpellWidth);
		}
		else {
			ActiveSpells.erase(it);
		}
	}
}

void Evade::OnProcessSpell(void* spellBook, SpellInfo* castInfo) {
	ActiveSpells.insert({ castInfo->StartTime, *castInfo });
}

void Evade::Initalize() {
	riot_render = (D3DRenderer*)*(DWORD*)DEFINE_RVA(Offsets::Data::D3DRender);
	EventManager::AddEventHandler(EventManager::EventType::OnProcessSpell, OnProcessSpell);
	EventManager::AddEventHandler(EventManager::EventType::OnDraw, OnDraw);
}
