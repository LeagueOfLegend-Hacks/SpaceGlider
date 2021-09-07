#include "OrbWalker.h"


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
static clock_t lastCast;
void OrbWalker::OrbWalk(GameObject* target, float extraWindup) {
	if (CanAttack() && target != nullptr) {
		if (Functions::IsAlive(target)) {
			auto w2s = riot_render->WorldToScreen(target->ServerPosition);
			Functions::IssueOrder(EOrderType::attack, w2s.x, w2s.y);
			LastAttackCommandT = GetTickCount64() - Functions::GetPing() / 2;
		}
	}
	if (CanMove(extraWindup) && LastMoveCommandT < GetTickCount64())
	{
		auto w2s = riot_render->WorldToScreen(Functions::GetMouseWorldPosition());
		Functions::IssueOrder(EOrderType::move, w2s.x, w2s.y);
		LastMoveCommandT = GetTickCount64() + 50;
	}
}

void OrbWalker::OnDraw(LPDIRECT3DDEVICE9 Device)
{
	auto pLocal = ObjectManager::GetLocalPlayer();
	auto target = TargetSelector::tryFindTarget(TargetSelector::TargetType::LowestHealth);

	static clock_t lastAntiAfk = 0;
	render.draw_circle(pLocal->Position, pLocal->AttackRange + pLocal->GetBoundingRadius(), ImColor(0, 255, 0, 255));
	if (GetAsyncKeyState(VK_SPACE)) {
		OrbWalk(target, 90.f);

		if(target != nullptr)
		render.draw_circle(target->Position, 50.f, ImColor(255, 0, 0, 255), ImRender::DrawType::Normal, 5.f);
	}

	/*
	else {
		if (lastAntiAfk == NULL || clock() - lastAntiAfk > 600) {
			lastAntiAfk = clock();
			auto w2s = riot_render->WorldToScreen(pLocal->ServerPosition)g;
			IssueClick(HUDInputLogic, 0, 0, 0, w2s.x, w2s.y, 0);
			IssueClick(HUDInputLogic, 1, 0, 0, w2s.x, w2s.y, 0);
		}
	}
	*/
}
void OrbWalker::OnProcessSpell(void* spellBook, SpellInfo* castInfo) {

	if (ObjectManager::GetLocalPlayer()->Index != castInfo->source_id)
		return;

	if (castInfo->Slot == kSpellSlot::SpellSlot_SpecialAttack)
	{
		LastAttackCommandT = 0;
		return;
	}


	if (castInfo->BasicAttackSpellData != nullptr)
		LastAttackCommandT = GetTickCount64() - Functions::GetPing() / 2;


}

void OrbWalker::Initalize() {
	Functions::IssueClick = (FuncTypes::fnIssueClick)(DEFINE_RVA(Offsets::Functions::IssueClick));
	LastMoveCommandT = 0;
	LastAttackCommandT = 0;
	riot_render = (D3DRenderer*)*(DWORD*)DEFINE_RVA(Offsets::Data::D3DRender);
	EventManager::AddEventHandler(EventManager::EventType::OnProcessSpell, OnProcessSpell);
	EventManager::AddEventHandler(EventManager::EventType::OnDraw, OnDraw);
}
