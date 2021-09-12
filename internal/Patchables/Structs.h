#pragma once
#include "Offsets.h"
#include "../Utils/Utils.h"
#include "../Vector.h"
#include "../Enums.h"
#include "../Patchables/CharacterData.h"
#include "../BuffManager/BuffManager.h"
#include "../SpellInstance/Spellbase.h"
#include <string>
#include <vector>
#include <algorithm>

struct SpellDataEntry {
	SpellType type;
	float duration = 0.0f;
	kDangerLevel DangerLevel = kDangerLevel::None;

};
struct MouseLockedPos {
	long x;
	long y;
	bool enabled;
};
extern MouseLockedPos MLP;
class SpellDataResource
{
public:
	union
	{
		DEFINE_MEMBER_N(float MissileSpeed, 0x454)
		DEFINE_MEMBER_N(float Width, 0x488)
		DEFINE_MEMBER_N(float Radius, 0x400)
	};
};
class SpellData
{
public:
	union
	{
		DEFINE_MEMBER_N(std::string Name, 0x18)
		DEFINE_MEMBER_N(SpellDataResource* Resource, 0x44)
	};
};

class AIManager
{
public:
	union
	{
		DEFINE_MEMBER_N(Vector3 NavStartPos, Offsets::AIManager::NavStart)
		DEFINE_MEMBER_N(Vector3 NavEndPos, Offsets::AIManager::NavEnd)
		DEFINE_MEMBER_N(Vector3 ServerPos, Offsets::AIManager::ServerPos)
		DEFINE_MEMBER_N(Vector3 Velocity, Offsets::AIManager::Velocity)
		DEFINE_MEMBER_N(bool Moving, Offsets::AIManager::Moving)
		DEFINE_MEMBER_N(bool Dashing, Offsets::AIManager::Dashing)
		DEFINE_MEMBER_N(float DashingSpeed, Offsets::AIManager::DashingSpeed)
		DEFINE_MEMBER_N(Vector3* NavArray, Offsets::AIManager::NavArray)
		DEFINE_MEMBER_N(Vector3* NavArrayEnd, Offsets::AIManager::NavArrayEnd)
	};

	int pathSize()
	{
		return ((DWORD)this->NavArrayEnd - (DWORD)this->NavArray) / (sizeof(Vector3));
	}

	float pathLength()
	{
		float length = 0;
		for (int i = 0; i < this->pathSize(); ++i)
			length += this->NavArray[i + 1].distance(this->NavArray[i]);

		return length;
	}
};

class SpellInfo {
public:
	union {
		DEFINE_MEMBER_0(SpellData* BasicAttackSpellData)
		DEFINE_MEMBER_N(kSpellSlot		Slot, Offsets::SpellInfo::Slot)
		DEFINE_MEMBER_N(float			StartTime, Offsets::SpellInfo::StartTime)
		DEFINE_MEMBER_N(int				SpellIndex, Offsets::SpellInfo::SpellIndex)
		DEFINE_MEMBER_N(unsigned int	Level, Offsets::SpellInfo::Level)
		DEFINE_MEMBER_N(unsigned short	source_id, Offsets::SpellInfo::source_id)
		DEFINE_MEMBER_N(unsigned int	SourceNetworkID, Offsets::SpellInfo::SourceNetworkID)
		DEFINE_MEMBER_N(Vector3			StartPosition, Offsets::SpellInfo::StartPosition)
		DEFINE_MEMBER_N(Vector3			EndPosition, Offsets::SpellInfo::EndPosition)
		DEFINE_MEMBER_N(bool			HasTarget, Offsets::SpellInfo::HasTarget)
		DEFINE_MEMBER_N(unsigned short	TargetId, Offsets::SpellInfo::TargetID)
	};
};

class GameObject {
public:
	union {
		DEFINE_MEMBER_0(DWORD* VTable)
		DEFINE_MEMBER_N(unsigned short	Index, Offsets::GameObject::Index)
		DEFINE_MEMBER_N(int				Team, Offsets::GameObject::TeamID)
		DEFINE_MEMBER_N(unsigned int	NetworkID, 0xCC)
		DEFINE_MEMBER_N(Vector3			Position, Offsets::GameObject::Position)
		DEFINE_MEMBER_N(bool			IsVisible, 0x23C)
		DEFINE_MEMBER_N(bool			IsTargetable, Offsets::GameObject::mIsTargetable)
		DEFINE_MEMBER_N(float			Health, Offsets::GameObject::HP)
		DEFINE_MEMBER_N(float			MaxHealth, Offsets::GameObject::MaxHP)
		DEFINE_MEMBER_N(float			Mana, Offsets::GameObject::Mana)
		DEFINE_MEMBER_N(float			Mana, Offsets::GameObject::MaxMana)
		DEFINE_MEMBER_N(float			Armor, Offsets::GameObject::Armor)
		DEFINE_MEMBER_N(float			BonusArmor, Offsets::GameObject::BonusArmor)
		DEFINE_MEMBER_N(float			AttackRange, Offsets::GameObject::AttackRange)
		DEFINE_MEMBER_N(float			BaseAttackDamage, Offsets::GameObject::BaseAttackDamage)
		DEFINE_MEMBER_N(float			BonusAttackDamage, Offsets::GameObject::BonusAttackDamage)
		DEFINE_MEMBER_N(float			MovementSpeed, Offsets::GameObject::MoveSpeed)
		DEFINE_MEMBER_N(char* ChampionName, Offsets::GameObject::ChampionName)
		DEFINE_MEMBER_N(SpellData* MissileSpellInfo, Offsets::GameObject::MissileSpellInfo)
		DEFINE_MEMBER_N(int				MissileSrcInx, Offsets::GameObject::MissileSrcIndex)
		DEFINE_MEMBER_N(Vector3			MissileStartPos, Offsets::GameObject::MissileStartPos)
		DEFINE_MEMBER_N(Vector3			MissileEndPos, Offsets::GameObject::MissileEndPos)
		DEFINE_MEMBER_N(int				MissileDestIdx, Offsets::GameObject::MissileDestIndex)
		DEFINE_MEMBER_N(CSpellBook		SpellBook, Offsets::GameObject::SpellBook)
		DEFINE_MEMBER_N(CharacterData* BaseCharacterData, Offsets::GameObject::BaseCharacterData)
		DEFINE_MEMBER_N(BuffManager BuffManager, Offsets::BuffManager::Instance);

	};

	AIManager* GameObject::GetAIManager() {
		return reinterpret_cast<AIManager * (__thiscall*)(GameObject*)>(this->VTable[149])(this);
	}

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

enum class EOrderType {
	move,
	attack
};
