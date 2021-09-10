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

int OrbWalker::GetGameTick()
{
	return (int)(*(float*)(DEFINE_RVA(Offsets::Data::GameTime)) * 1000);
}

bool OrbWalker::CanAttack() {
	return GetGameTick() + Functions::GetPing() / 2.f >= LastAttackCommandT + Functions::GetAttackDelay(ObjectManager::GetLocalPlayer()) * 1000.f;
}

bool OrbWalker::CanMove(float extraWindup) {
	return GetGameTick() >= LastAttackCommandT + Functions::GetAttackCastDelay(ObjectManager::GetLocalPlayer()) * 1000.f + Functions::GetPing() / 2 + extraWindup || ObjectManager::GetLocalPlayer()->GetChampionName() == "Kalista";
}
static clock_t lastCast;
void OrbWalker::OrbWalk(GameObject* target, float extraWindup) {
	if (CanAttack() && target != nullptr) {
		if (Functions::IsAlive(target)) {
			auto w2s = riot_render->WorldToScreen(target->Position);
			Functions::IssueOrder(EOrderType::attack, w2s.x, w2s.y);
			LastAttackCommandT = GetGameTick() - Functions::GetPing() / 2;
		}
	}
	if (CanMove(extraWindup) && LastMoveCommandT < GetGameTick())
	{
		auto w2s = riot_render->WorldToScreen(Functions::GetMouseWorldPosition());
		Functions::IssueOrder(EOrderType::move, w2s.x, w2s.y);
		LastMoveCommandT = GetGameTick() + 50;
	}
}

void OrbWalker::OnDraw()
{
	auto target = TargetSelector::tryFindTarget(TargetSelector::TargetType::LowestHealth, ObjectManager::HeroList());

	if (GetAsyncKeyState(VK_SPACE)) {
		OrbWalk(target, 90.f);
	}
	else if (GetAsyncKeyState(0x56)) {
		auto ObjectList = ObjectManager::MinionList();
		ObjectList.merge(ObjectManager::BuildingList());
		target = TargetSelector::tryFindTarget(TargetSelector::TargetType::LowestHealth, ObjectList);
		OrbWalk(target, 90.0f);
	}

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
		LastAttackCommandT = GetGameTick() - Functions::GetPing() / 2;


}

void OrbWalker::Initialize() {
	Functions::IssueClick = (FuncTypes::fnIssueClick)(DEFINE_RVA(Offsets::Functions::IssueClick));
	EventManager::AddEventHandler(EventManager::EventType::OnProcessSpell, OnProcessSpell);
	EventManager::AddEventHandler(EventManager::EventType::OnDraw, OnDraw);

	EventManager::AddEventHandler(EventManager::EventType::OnLoad, OnLoad);
	EventManager::AddEventHandler(EventManager::EventType::OnUnLoad, OnUnload);
}

void OrbWalker::OnLoad()
{
	LastMoveCommandT = 0;
	LastAttackCommandT = 0;
}

void OrbWalker::OnUnload()
{
	EventManager::RemoveEventHandler(EventManager::EventType::OnProcessSpell, OnProcessSpell);
	EventManager::AddEventHandler(EventManager::EventType::OnDraw, OnDraw);
}