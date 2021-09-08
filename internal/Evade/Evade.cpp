#include "Evade.h"

std::unordered_map<float, SpellInfo> Evade::ActiveSpells;
std::unordered_map<std::string, SpellDataEntry> Evade::SpellData;

void Evade::OnDraw(LPDIRECT3DDEVICE9 Device) {
	std::unordered_map<float, SpellInfo>::iterator it;
	for (it = ActiveSpells.begin(); it != ActiveSpells.end(); it++) {
		auto spellData = SpellData.find(it->second.BasicAttackSpellData->Name);
		if (spellData != SpellData.end()) {
			float duration = spellData->second.type == SpellType::Line ? it->second.StartPosition.distance(it->second.EndPosition) / it->second.BasicAttackSpellData->Resource->MissileSpeed : spellData->second.duration;
			if (it->second.StartTime + duration >= *(float*)(DEFINE_RVA(Offsets::Data::GameTime))) {
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
				auto Poly = Geometry::Rectangle(it->second.StartPosition, it->second.EndPosition, it->second.BasicAttackSpellData->Resource->Width).ToPolygon(5);
				switch (spellData->second.type) {
				case SpellType::Line:
					for (size_t i = 0; i <= Poly.Points.size() - 1; i++)
					{
						auto iNextIdx = (Poly.Points.size() - 1 == i) ? 0 : (i + 1);
						StartPos_W2S = riot_render->WorldToScreen(Poly.Points[i]);
						EndPos_W2S = riot_render->WorldToScreen(Poly.Points[iNextIdx]);
						render.draw_line(StartPos_W2S, EndPos_W2S, Color, 5);
					}
					break;
				case SpellType::Circle:
					render.draw_circle(it->second.EndPosition, it->second.BasicAttackSpellData->Resource->Radius, Color, ImRender::DrawType::Filled);
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
}

void Evade::OnProcessSpell(void* spellBook, SpellInfo* castInfo) {
	ActiveSpells.insert({ castInfo->StartTime, *castInfo });
}

void Evade::Initalize() {
	SpellData.insert({ "MorganaQ", { SpellType::Line, 1.2, kDangerLevel::High}});
	SpellData.insert({ "MorganaW", { SpellType::Circle, 5, kDangerLevel::Low}});
	riot_render = (D3DRenderer*)*(DWORD*)DEFINE_RVA(Offsets::Data::D3DRender);
	EventManager::AddEventHandler(EventManager::EventType::OnProcessSpell, OnProcessSpell);
	EventManager::AddEventHandler(EventManager::EventType::OnDraw, OnDraw);
}
