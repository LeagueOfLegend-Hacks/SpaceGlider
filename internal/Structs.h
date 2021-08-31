#pragma once
#include "Offsets.h"
#include "Utils.h"
#include "Vector.h"
#include "Enums.h"
// Custom
struct MouseLockedPos {
	Vector2 Position;
	bool Enabled;
};
// League Specific
class SpellInfo {
public:
	union {
		DEFINE_MEMBER_0(DWORD*			VTable)
		DEFINE_MEMBER_N(kSpellSlot		Slot,				0x4)
		DEFINE_MEMBER_N(float			StartTime,			0x8)
		DEFINE_MEMBER_N(int				SpellIndex,			0xC)
		DEFINE_MEMBER_N(unsigned int	Level,				0x58)
		DEFINE_MEMBER_N(unsigned short	source_id,			0x64)
		DEFINE_MEMBER_N(unsigned int	SourceNetworkID,	0x6C)
		DEFINE_MEMBER_N(Vector3			StartPosition,		0x7C)
		DEFINE_MEMBER_N(Vector3			EndPosition,		0x88)
		DEFINE_MEMBER_N(Vector3			CastPos,			0x94)
		DEFINE_MEMBER_N(bool			HasTarget,			0xB4)
		DEFINE_MEMBER_N(unsigned short	TargetId,			0xB8)
	};
};
class GameObject {
public:
	union {
		DEFINE_MEMBER_0(DWORD*			VTable)
		DEFINE_MEMBER_N(unsigned short	Index,													Offsets::GameObject::Index)
		DEFINE_MEMBER_N(int				Team,													Offsets::GameObject::TeamID)
		DEFINE_MEMBER_N(unsigned int	NetworkID,												0xCC)
		DEFINE_MEMBER_N(byte			IsOnScreen,												0x1A8)
		DEFINE_MEMBER_N(Vector3			ServerPosition,											0x1D8)
		DEFINE_MEMBER_N(Vector3			Position,												Offsets::GameObject::Position)
		DEFINE_MEMBER_N(bool			IsVisible,												0x23C)
		DEFINE_MEMBER_N(bool			IsTargetable,											Offsets::GameObject::mIsTargetable)
		DEFINE_MEMBER_N(float			Health,													Offsets::GameObject::HP)
		DEFINE_MEMBER_N(float			MaxHealth,												Offsets::GameObject::MaxHP)
		DEFINE_MEMBER_N(float			Armor,													Offsets::GameObject::Armor)
		DEFINE_MEMBER_N(float			BonusArmor,												Offsets::GameObject::BonusArmor)
		DEFINE_MEMBER_N(float			AttackRange,											Offsets::GameObject::AttackRange)
		DEFINE_MEMBER_N(float			BaseAttackDamage,										Offsets::GameObject::BaseAttackDamage)
		DEFINE_MEMBER_N(float			BonusAttackDamage,										0x121C)
		DEFINE_MEMBER_N(char*			ChampionName,											0x2BB4)
		// all these are unused but we grab them from ida so, might as well put them in
		DEFINE_MEMBER_N(BYTE			PAR,													Offsets::GameObject::mPAR)
		DEFINE_MEMBER_N(BYTE			MaxPar,													Offsets::GameObject::mMaxPAR)
		DEFINE_MEMBER_N(BYTE			ParEnabled,												Offsets::GameObject::mPAREnabled)
		DEFINE_MEMBER_N(BYTE			ParState,												Offsets::GameObject::mPARState)
		DEFINE_MEMBER_N(BYTE			SAR,													Offsets::GameObject::mSAR)
		DEFINE_MEMBER_N(BYTE			MaxSAR,													Offsets::GameObject::mMaxSAR)
		DEFINE_MEMBER_N(BYTE			SAREnabled,												Offsets::GameObject::mSAREnabled)
		DEFINE_MEMBER_N(BYTE			LargePipBitField,										Offsets::GameObject::LargePipBitField)
		DEFINE_MEMBER_N(BYTE			MediumPipBitField,										Offsets::GameObject::MediumPipBitField)
		DEFINE_MEMBER_N(BYTE			SARState,												Offsets::GameObject::mSARState)
		DEFINE_MEMBER_N(BYTE			LifeTime,												Offsets::GameObject::mLifetime)
		DEFINE_MEMBER_N(BYTE			LifeTimeTicks,											Offsets::GameObject::mLifetimeTicks)
		DEFINE_MEMBER_N(BYTE			PhysicalDamagePercentageModifier,						Offsets::GameObject::PhysicalDamagePercentageModifier)
	};
};