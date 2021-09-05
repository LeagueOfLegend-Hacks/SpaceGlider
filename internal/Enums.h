#pragma once
enum kSpellSlot
{
	SpellSlot_Unknown = -1,
	SpellSlot_Q,
	SpellSlot_W,
	SpellSlot_E,
	SpellSlot_R,
	SpellSlot_Summoner1,
	SpellSlot_Summoner2,
	SpellSlot_Item1,
	SpellSlot_Item2,
	SpellSlot_Item3,
	SpellSlot_Item4,
	SpellSlot_Item5,
	SpellSlot_Item6,
	SpellSlot_Trinket,
	SpellSlot_Recall,
	SpellSlot_SpecialAttack = 45,
	SpellSlot_BasicAttack1 = 64,
	SpellSlot_BasicAttack2 = 65,
};
enum class GameObjectOrder
{
	None = 0,
	HoldPosition,
	MoveTo,
	AttackUnit,
	AutoAttackPet,
	AutoAttack,
	MovePet,
	AttackTo,
	Stop = 10,
};
enum MouseSetting {
	DOWN,
	UP,
	HOLD,
};
enum class SpellType {
	Line,
	Circle,
	Arc3
};