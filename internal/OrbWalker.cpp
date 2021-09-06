#include "OrbWalker.h"
#include "TargetSelector.h"
#include "Console.h"
#include <ctime>
#include "XorStr.h"

float OrbWalker::LastAttackCommandT = 0;
float OrbWalker::LastMoveCommandT = 0;

std::string OrbWalker::AttackResets[] =
{
	xorstr("dariusnoxiantacticsonh"), xorstr("fiorae"), xorstr("garenq"), xorstr("gravesmove"),
	xorstr("hecarimrapidslash"), xorstr("jaxempowertwo"), xorstr("jaycehypercharge"),
	xorstr("leonashieldofdaybreak"), xorstr("luciane"), xorstr("monkeykingdoubleattack"),
	xorstr("mordekaisermaceofspades"), xorstr("nasusq"), xorstr("nautiluspiercinggaze"),
	xorstr("netherblade"), xorstr("gangplankqwrapper"), xorstr("powerfist"),
	xorstr("renektonpreexecute"), xorstr("rengarq"), xorstr("shyvanadoubleattack"),
	xorstr("sivirw"), xorstr("takedown"), xorstr("talonnoxiandiplomacy"),
	xorstr("trundletrollsmash"), xorstr("vaynetumble"), xorstr("vie"), xorstr("volibearq"),
	xorstr("xenzhaocombotarget"), xorstr("yorickspectral"), xorstr("reksaiq"),
	xorstr("itemtitanichydracleave"), xorstr("masochism"), xorstr("illaoiw"),
	xorstr("elisespiderw"), xorstr("fiorae"), xorstr("meditate"), xorstr("sejuaninorthernwinds"),
	xorstr("asheq"), xorstr("camilleq"), xorstr("camilleq2")
};

bool OrbWalker::CanAttack() {
	return GetTickCount64() + Functions::GetPing() / 2.f >= LastAttackCommandT + Functions::GetAttackDelay(ObjectManager::GetLocalPlayer()) * 1000.f;
}

bool OrbWalker::CanMove(float extraWindup) {
	return GetTickCount64() >= LastAttackCommandT + Functions::GetAttackCastDelay(ObjectManager::GetLocalPlayer()) * 1000.f + Functions::GetPing() + extraWindup || ObjectManager::GetLocalPlayer()->GetChampionName() == "Kalista";
}

void OrbWalker::OrbWalk(GameObject* target, float extraWindup) {
	DWORD HUDInputLogic = *(DWORD*)(*(DWORD*)DEFINE_RVA(Offsets::Data::HudInstance) + 0x24);
	typedef int(__thiscall* fnIssueClick)(int thisptr, int State, int IsAttack, int unknown2, int screen_x, int screen_y, char unknown3);
	fnIssueClick IssueClick = (fnIssueClick)(DEFINE_RVA(0x5AE550));
	if (CanAttack() && target != nullptr) {
		if (Functions::IsAlive(target)) {
			auto w2s = riot_render->WorldToScreen(target->ServerPosition);
			IssueClick(HUDInputLogic, 0, 1, 0, w2s.x, w2s.y, 0);
			IssueClick(HUDInputLogic, 1, 1, 0, w2s.x, w2s.y, 0);


		}
	}
	if (CanMove(extraWindup) && LastMoveCommandT < GetTickCount64())
	{
		auto w2s = riot_render->WorldToScreen(Functions::GetMouseWorldPosition());
		IssueClick(HUDInputLogic, 0, 0, 0, w2s.x, w2s.y, 0);
		IssueClick(HUDInputLogic, 1, 0, 0, w2s.x, w2s.y, 0);
		LastMoveCommandT = GetTickCount64() + 50;
	}
}

void OrbWalker::OnDraw(LPDIRECT3DDEVICE9 Device)
{
	DWORD HUDInputLogic = *(DWORD*)(*(DWORD*)DEFINE_RVA(Offsets::Data::HudInstance) + 0x24);
	typedef int(__thiscall* fnIssueClick)(int thisptr, int State, int IsAttack, int unknown2, int screen_x, int screen_y, char unknown3);
	fnIssueClick IssueClick = (fnIssueClick)(DEFINE_RVA(0x5AE550));
	auto pLocal = ObjectManager::GetLocalPlayer();
	static clock_t lastAntiAfk = 0;
	render.draw_circle(pLocal->Position, pLocal->AttackRange + pLocal->GetBoundingRadius(), ImColor(0, 255, 0, 255));
	if (GetAsyncKeyState(VK_SPACE)) {
		OrbWalk(TargetSelector::tryFindTarget(TargetSelector::TargetType::LowestHealth), 90.f);
	}
	
	/*
	else {
		if (lastAntiAfk == NULL || clock() - lastAntiAfk > 600) {
			lastAntiAfk = clock();
			auto w2s = riot_render->WorldToScreen(pLocal->ServerPosition);
			IssueClick(HUDInputLogic, 0, 0, 0, w2s.x, w2s.y, 0);
			IssueClick(HUDInputLogic, 1, 0, 0, w2s.x, w2s.y, 0);
		}
	}
	*/
}
void OrbWalker::OnProcessSpell(void* spellBook, SpellInfo* castInfo) {
	if (ObjectManager::GetLocalPlayer()->Index == castInfo->source_id) {

		if (castInfo->Slot == kSpellSlot::SpellSlot_SpecialAttack)
		{
			LastAttackCommandT = 0;
			return;
		}


		for each (std::string var in AttackResets)
		{
			if (castInfo->BasicAttackSpellData->Name.compare(var))
			{
				LastAttackCommandT = 0;
				break;
			}

		}

		if (castInfo->BasicAttackSpellData != nullptr)
			LastAttackCommandT = GetTickCount64() - Functions::GetPing() / 2;

		/*
		switch (castInfo->Slot) {
		case kSpellSlot::SpellSlot_Unknown:
		case kSpellSlot::SpellSlot_BasicAttack1:
		case kSpellSlot::SpellSlot_BasicAttack2:
		case kSpellSlot::SpellSlot_SpecialAttack:
			LastAttackCommandT = float(GetTickCount64()) + GetPing() / 2;
			break;
		}
		*/
	}
}

void OrbWalker::Initalize() {
	LastMoveCommandT = 0;
	LastAttackCommandT = 0;
	riot_render = (D3DRenderer*)*(DWORD*)DEFINE_RVA(Offsets::Data::D3DRender);
	EventManager::AddEventHandler(EventManager::EventType::OnProcessSpell, OnProcessSpell);
	EventManager::AddEventHandler(EventManager::EventType::OnDraw, OnDraw);
}
