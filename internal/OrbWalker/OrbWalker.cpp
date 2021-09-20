#include "OrbWalker.h"

std::string Orbwalking::AttackResets[] =
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

bool Orbwalking::CanAttack() {
	return Functions::GameTimeTick() + Functions::GetPing() / 2.f >= LastAttackCommandT + Functions::GetAttackDelay(Local) * 1000.f;
}

bool Orbwalking::CanMove(float extraWindup) {
	return Functions::GameTimeTick() >= LastAttackCommandT + Functions::GetAttackCastDelay(Local) * 1000.f + Functions::GetPing() / 2 + extraWindup || Local->GetChampionName() == "Kalista";
}

void Orbwalking::Orbwalk(GameObject* target, float extraWindup) {
	if (Functions::GameTimeTick() - LastAttackCommandT < 70 + min(60, Functions::GetPing()))
	{
		return;
	}

	if (target && CanAttack()) {
		Vector2 w2s = riot_render->WorldToScreen(target->Position);
		Functions::IssueOrder(EOrderType::attack, w2s.x, w2s.y);
		LastAttackCommandT = Functions::GameTimeTick() - Functions::GetPing() / 2;
	}
	if (CanMove(extraWindup) && LastMoveCommandT < Functions::GameTimeTick())
	{
		Vector2 w2s = riot_render->WorldToScreen(Functions::GetMouseWorldPosition());
		Functions::IssueOrder(EOrderType::move, w2s.x, w2s.y);
		LastMoveCommandT = Functions::GameTimeTick() + 90;
	}
}

void Orbwalking::OnUpdate()
{
	if (!Helper::isValidUnit(_prevMinion))
		_prevMinion = nullptr;

	if (!Functions::IsAlive(Local))
		_mode = OrbwalkingMode::None;

	if (Evade::Core::Evading || Evade::Core::IsDangerous(Evade::Core::PreviousPos))
	{
		_mode = OrbwalkingMode::None;
		return;
	}

	if (GetAsyncKeyState(VK_SPACE)) {
		_mode = OrbwalkingMode::Combo;
	}
	else if (GetAsyncKeyState(0x56)) {
		_mode = OrbwalkingMode::LaneClear;
	}
	else if (GetAsyncKeyState(0x58)) {
		_mode = OrbwalkingMode::LastHit;
	}
	else
	{
		_mode = OrbwalkingMode::None;
	}

	try
	{
		if (_mode == OrbwalkingMode::None)
		{
			return;
		}

		MinionListAA = filter(ObjectManager::MinionList(), [&](GameObject* minion) 
			{ 
				if (!Helper::isValidUnit(minion) || !minion->IsInRange(Local, Local->AttackRange, true))
					return false;
				return true;
			});

		Orbwalk(GetTarget(), 90);
	}
	catch (...)
	{
		console.Print("Orbwalking delayed");
	}
}

float Orbwalking::GetMyProjectileSpeed()
{
	return Local->IsMelee() || Local->GetChampionName() == "Azir" || Local->GetChampionName() == "Thresh" || Local->GetChampionName() == "Velkoz" ||
		Local->GetChampionName() == "Viktor" && Local->BuffManager.hasBuff("ViktorPowerTransferReturn")
		? FLT_MAX
		: Functions::GetBasicAttack(Local)->Resource->MissileSpeed;
}

bool Orbwalking::ShouldWait()
{
	int attackCalc = (int)(Functions::GetAttackDelay(Local) * 1000 * 1.6) + Functions::GetPing() / 2 + 1000 * 500 / (int)GetMyProjectileSpeed();
	for (auto minion : MinionListAA)
	{
		if (HealthPrediction::LaneClearHealthPrediction(minion, attackCalc, 0) <= Local->BaseAttackDamage + Local->BonusAttackDamage)
			return true;
	}
	return false;
}

GameObject* Orbwalking::GetTarget()
{
	GameObject* result = nullptr;

	if (_mode == OrbwalkingMode::Mixed || _mode == OrbwalkingMode::LaneClear)
	{
		GameObject* target = TargetSelector::GetTarget(Local->AttackRange, DamageType::Physical);
		if (target != nullptr)
			return target;
	}
	if (_mode == OrbwalkingMode::LaneClear || _mode == OrbwalkingMode::Mixed || _mode == OrbwalkingMode::LastHit || _mode == OrbwalkingMode::Freeze)
	{
		auto newList = MinionListAA;
		newList.sort([&](GameObject* first, GameObject* next) {
			return HealthPrediction::GetHealthPrediction(first, 1200) < HealthPrediction::GetHealthPrediction(next, 1200) && first->MaxHealth > next->MaxHealth;
			});
		for (auto minion : newList)
		{
			if (!minion->IsNeutral())
			{
				float t = (int)(Functions::GetAttackCastDelay(Local) * 1000) + BrainFarmInt + Functions::GetPing() / 2 + 1000 * (int)max(0, Local->Position.Distance(minion->Position) - Local->GetBoundingRadius()) / (int)GetMyProjectileSpeed();

				if (_mode == OrbwalkingMode::Freeze)
					t += 200 + Functions::GetPing() / 2;

				float predHealth = HealthPrediction::GetHealthPrediction(minion, t, 0);

				bool killable = predHealth <= Local->BaseAttackDamage + Local->BonusAttackDamage;
				if (_mode == OrbwalkingMode::Freeze)
				{
					if (minion->Health + (minion->Armor + minion->BonusArmor) < 50 || predHealth <= 50)
					{
						return minion;
					}
				}
				else
				{
					if (CanAttack())
					{

						DelayOnFire = t + Functions::GameTimeTick();
						DelayOnFireId = minion->NetworkID;
					}

					if (predHealth <= 0)
					{
						if (HealthPrediction::GetHealthPrediction(minion, t - 50, 0) > 0)
						{
							return minion;
						}
					}

					else if (killable)
					{
						return minion;
					}
				}
			}
		}
	}
	if (CanAttack())
	{
		DelayOnFire = 0;
	}

	if (_mode != OrbwalkingMode::LastHit)
	{
		GameObject* target = TargetSelector::GetTarget(Local->AttackRange, DamageType::Physical);
		if (Helper::isValidUnit(target) && target->IsInRange(Local, Local->AttackRange, true))
		{
			return target;
		}
	}

	if (_mode == OrbwalkingMode::LaneClear || _mode == OrbwalkingMode::Mixed) {
		for (GameObject* building : filter(ObjectManager::TurretList(), [&](GameObject* Building) {return Helper::isValidUnit(Building, Local->AttackRange) && Local->IsInRange(Building, Local->AttackRange, true); }))
			return building;
	}

	if (_mode == OrbwalkingMode::LaneClear)
	{
		if (!ShouldWait())
		{
			if (_prevMinion && Helper::isValidUnit(_prevMinion) && _prevMinion->IsInRange(Local, Local->AttackRange, true))
			{
				float predHealth = HealthPrediction::LaneClearHealthPrediction(_prevMinion, (int)(Functions::GetAttackDelay(Local) * 1000 * 2.f), 0);
				if (predHealth >= 2 * Local->BaseAttackDamage + Local->BonusAttackDamage || abs(predHealth - _prevMinion->Health) < FLT_EPSILON)
					return _prevMinion;
			}

			MinionListAA.sort([&](GameObject* first, GameObject* next) {
				return first->MaxHealth > next->MaxHealth;
				});
			for (auto minion : MinionListAA)
			{
				if (!Functions::IsMinion(minion))
					continue;
				float predHealth = HealthPrediction::LaneClearHealthPrediction(minion, (int)(Functions::GetAttackDelay(Local) * 1000 * 2.f, 0));
				if (predHealth >= 2 * Local->BaseAttackDamage + Local->BonusAttackDamage || abs(predHealth - minion->Health) < FLT_EPSILON)
					result = minion;
			}

			if (result)
				_prevMinion = result;
		}
	}

	return result;
}

void Orbwalking::OnDraw()
{
	OnUpdate();
	render.draw_circle(Local->Position, Local->AttackRange + Local->GetBoundingRadius(), ImColor(0, 255, 0));
}
void Orbwalking::OnProcessSpell(void* spellBook, SpellInfo* castInfo) {

	if (Local->Index != castInfo->source_id)
		return;

	if (castInfo->Slot == kSpellSlot::SpellSlot_SpecialAttack)
	{
		LastAttackCommandT = 0;
		return;
	}

	if (castInfo->BasicAttackSpellData != nullptr)
		LastAttackCommandT = Functions::GameTimeTick() - Functions::GetPing() / 2;

}

void Orbwalking::OnDeleteObject(void* thisPtr, GameObject* obj) 
{}

void Orbwalking::Initialize() {
	Functions::IssueClick = (FuncTypes::fnIssueClick)(DEFINE_RVA(Offsets::Functions::IssueClick));
	EventManager::AddEventHandler(EventManager::EventType::OnProcessSpell, OnProcessSpell);
	EventManager::AddEventHandler(EventManager::EventType::OnDeleteObject, OnDeleteObject);
	EventManager::AddEventHandler(EventManager::EventType::OnDraw, OnDraw);

	EventManager::AddEventHandler(EventManager::EventType::OnLoad, OnLoad);
	EventManager::AddEventHandler(EventManager::EventType::OnUnLoad, OnUnload);
}

void Orbwalking::OnLoad()
{
	LastMoveCommandT = 0;
	LastAttackCommandT = 0;
}

void Orbwalking::OnUnload()
{
	EventManager::RemoveEventHandler(EventManager::EventType::OnProcessSpell, OnProcessSpell);
	EventManager::AddEventHandler(EventManager::EventType::OnDeleteObject, OnDeleteObject);
	EventManager::AddEventHandler(EventManager::EventType::OnDraw, OnDraw);
}