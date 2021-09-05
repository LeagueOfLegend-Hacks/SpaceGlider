#pragma once
#include "Offsets.h"
#include "Utils.h"
#include "Vector.h"
#include "Enums.h"
#include <string>
struct SpellDataEntry {
	int width;
	SpellType type;
	float duration;
};
class SpellData
{
public:
	union
	{
		DEFINE_MEMBER_N(std::string Name, 0x18)
	};
};
class SpellInfo {
public:
	union {
		DEFINE_MEMBER_0(SpellData*		BasicAttackSpellData)
		DEFINE_MEMBER_N(kSpellSlot		Slot,				Offsets::SpellInfo::Slot)
		DEFINE_MEMBER_N(float			StartTime,			Offsets::SpellInfo::StartTime)
		DEFINE_MEMBER_N(int				SpellIndex,			Offsets::SpellInfo::SpellIndex)
		DEFINE_MEMBER_N(unsigned int	Level,				Offsets::SpellInfo::Level)
		DEFINE_MEMBER_N(unsigned short	source_id,			Offsets::SpellInfo::source_id)
		DEFINE_MEMBER_N(unsigned int	SourceNetworkID,	Offsets::SpellInfo::SourceNetworkID)
		DEFINE_MEMBER_N(Vector3			StartPosition,		Offsets::SpellInfo::StartPosition)
		DEFINE_MEMBER_N(Vector3			EndPosition,		Offsets::SpellInfo::EndPosition)
		DEFINE_MEMBER_N(bool			HasTarget,			Offsets::SpellInfo::HasTarget)
		DEFINE_MEMBER_N(unsigned short	TargetId,			Offsets::SpellInfo::TargetID)
	};
};
class SpellBook {
public:
	union {
		DEFINE_MEMBER_0(DWORD* VTable)
		DEFINE_MEMBER_N(DWORD*			Info,													Offsets::Spell::kInvalid)
	};
};
class GameObject {
public:
	union {
		DEFINE_MEMBER_0(DWORD*			VTable)
		DEFINE_MEMBER_N(unsigned short	Index,													Offsets::GameObject::Index)
		DEFINE_MEMBER_N(int				Team,													Offsets::GameObject::TeamID)
		DEFINE_MEMBER_N(unsigned int	NetworkID,												0xCC)
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
		DEFINE_MEMBER_N(SpellData*		MissileSpellInfo, 0x258)
		DEFINE_MEMBER_N(int				MissileSrcInx, 0x2BC)
		DEFINE_MEMBER_N(Vector3			MissileStartPos, 0x2D4)
		DEFINE_MEMBER_N(Vector3			MissileEndPos, 0x2E0)
		DEFINE_MEMBER_N(int				MissileDestIdx, 0x314)
		DEFINE_MEMBER_N(SpellBook		SpellBook,												0x2350)
	};
	float GameObject::GetBoundingRadius() {
		return reinterpret_cast<float(__thiscall*)(GameObject*)>(this->VTable[35])(this);
	}
	bool IsAllyTo(GameObject* Obj) {
		return Obj->Team == this->Team;
	}
	bool IsNeutral() {
		return this->Team == 300;
	}
	bool IsEnemyTo(GameObject* Obj) {
		if (this->IsNeutral() || Obj->IsNeutral())
			return false;
		return !IsAllyTo(Obj);
	}
	std::string GetChampionName() {
		return std::string(this->ChampionName);
	}
};
template<typename T>
struct SEntityList {
	char pad[0x4];
	T** entities;
	size_t size;
	size_t max_size;
};