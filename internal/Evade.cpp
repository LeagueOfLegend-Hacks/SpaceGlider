#include "Evade.h"

std::unordered_map<float, SpellInfo> Evade::ActiveSpells;
std::unordered_map<std::string, SpellDataEntry> Evade::SpellData;

void Evade::OnDraw(LPDIRECT3DDEVICE9 Device) {
	std::unordered_map<float, SpellInfo>::iterator it;
	for (it = ActiveSpells.begin(); it != ActiveSpells.end(); it++) {
		auto spellData = SpellData.find(it->second.BasicAttackSpellData->Name);
		if (spellData != SpellData.end()) {
			if (it->second.StartTime + spellData->second.duration > *(float*)(DEFINE_RVA(Offsets::Data::GameTime))) {
				Vector2 StartPos_W2S, EndPos_W2S;
				StartPos_W2S = riot_render->WorldToScreen(it->second.StartPosition);
				EndPos_W2S = riot_render->WorldToScreen(it->second.EndPosition);

				auto Caster = ObjectManager::FindObjectByIndex(ObjectManager::HeroList(), it->second.source_id);

				auto SpellWidth = 10.0f;

				float Alpha = (float)spellData->second.DangerLevel / 10;
				ImColor Color = ImColor(1.0f, 1.0f, 1.0f, Alpha);
				if (Caster->IsEnemyTo(ObjectManager::GetLocalPlayer()))
					Color = ImColor(1.0f, 0.0f, 0.0f, Alpha);
				else
					Color = ImColor(0.0f, 1.0f, 0.0f, Alpha);

				switch (spellData->second.type) {
				case SpellType::Line:
					render.draw_line(StartPos_W2S, EndPos_W2S, Color, spellData->second.width);
					break;
				case SpellType::Circle:
					render.draw_circle(it->second.EndPosition, spellData->second.width, Color, ImRender::DrawType::Filled);
					break;
				}
			}
			else {
				ActiveSpells.erase(it);
			}
		}
		else {
			ActiveSpells.erase(it);
		}
	}
	render.draw_text({ 5, 5 }, std::to_string(ActiveSpells.size()).c_str());
}

void Evade::OnProcessSpell(void* spellBook, SpellInfo* castInfo) {
	ActiveSpells.insert({ castInfo->StartTime, *castInfo });
}

void Evade::Initalize() {
	SpellData.insert({ "MorganaQ", { 100, SpellType::Line, 1.2, kDangerLevel::High}});
	SpellData.insert({ "MorganaW", { 275, SpellType::Circle, 5, kDangerLevel::Low}});
	riot_render = (D3DRenderer*)*(DWORD*)DEFINE_RVA(Offsets::Data::D3DRender);
	EventManager::AddEventHandler(EventManager::EventType::OnProcessSpell, OnProcessSpell);
	EventManager::AddEventHandler(EventManager::EventType::OnDraw, OnDraw);
}
