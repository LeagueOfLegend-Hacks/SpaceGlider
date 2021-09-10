#pragma once
#include "Offsets.h"
#include "../Utils/Utils.h"
#include "../Vector.h"
#include "../Enums.h"
#include "../Patchables/CharacterData.h"
#include <string>

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
		//DEFINE_MEMBER_N(char* MissileName, Offsets::SpellDataResource::MissileName)
		//DEFINE_MEMBER_N(char* SpellName, Offsets::SpellDataResource::SpellName)
		//DEFINE_MEMBER_N(const char* DisplayNameLocalizationKey, Offsets::SpellDataResource::DisplayNameLocalizationKey)
		//DEFINE_MEMBER_N(float eEffect1[7], Offsets::SpellDataResource::eEffect1)
		//DEFINE_MEMBER_N(float eEffect2[7], Offsets::SpellDataResource::eEffect2)
		//DEFINE_MEMBER_N(float eEffect3[7], Offsets::SpellDataResource::eEffect3)
		//DEFINE_MEMBER_N(float eEffect4[7], Offsets::SpellDataResource::eEffect4)
		//DEFINE_MEMBER_N(float eEffect5[7], Offsets::SpellDataResource::eEffect5)
		//DEFINE_MEMBER_N(float eEffect6[7], Offsets::SpellDataResource::eEffect6)
		//DEFINE_MEMBER_N(float eEffect7[7], Offsets::SpellDataResource::eEffect7)
		//DEFINE_MEMBER_N(float eEffect8[7], Offsets::SpellDataResource::eEffect8)
		//DEFINE_MEMBER_N(float eEffect9[7], Offsets::SpellDataResource::eEffect9)
		//DEFINE_MEMBER_N(float eEffect10[7], Offsets::SpellDataResource::eEffect10)
		//DEFINE_MEMBER_N(float eEffect11[7], Offsets::SpellDataResource::eEffect11)
		//DEFINE_MEMBER_N(float aEffect1, Offsets::SpellDataResource::aEffect1)
		//DEFINE_MEMBER_N(float aEffect2, Offsets::SpellDataResource::aEffect2)
		//DEFINE_MEMBER_N(float CastTime, Offsets::SpellDataResource::CastTime)
		//DEFINE_MEMBER_N(float CastDelay, Offsets::SpellDataResource::CastDelay)
		//DEFINE_MEMBER_N(bool CantCancelWhileWindingUp, Offsets::SpellDataResource::CantCancelWhileWindingUp)
		//DEFINE_MEMBER_N(bool CantCancelWhileChanneling, Offsets::SpellDataResource::CantCancelWhileChanneling)
		//DEFINE_MEMBER_N(bool ChannelIsInterruptedByAttacking, Offsets::SpellDataResource::ChannelIsInterruptedByAttacking)
		//DEFINE_MEMBER_N(bool CanMoveWhileChanneling, Offsets::SpellDataResource::CanMoveWhileChanneling)
		DEFINE_MEMBER_N(float MissileSpeed, 0x454)
			DEFINE_MEMBER_N(float Width, 0x488)
			DEFINE_MEMBER_N(float Radius, 0x400)
			//DEFINE_MEMBER_N(float EffectRange, Offsets::SpellDataResource::SpellDataEffectSpellRange)
			//DEFINE_MEMBER_N(float AfterEffectRange, Offsets::SpellDataResource::SpellDataEffectSpellRangeAfterEffect)
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
class SpellBook {
public:
	union {
		DEFINE_MEMBER_0(DWORD* VTable)
			DEFINE_MEMBER_N(DWORD* Info, Offsets::Spell::kInvalid)
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
			DEFINE_MEMBER_N(float			Armor, Offsets::GameObject::Armor)
			DEFINE_MEMBER_N(float			BonusArmor, Offsets::GameObject::BonusArmor)
			DEFINE_MEMBER_N(float			AttackRange, Offsets::GameObject::AttackRange)
			DEFINE_MEMBER_N(float			BaseAttackDamage, Offsets::GameObject::BaseAttackDamage)
			DEFINE_MEMBER_N(float			BonusAttackDamage, Offsets::GameObject::BonusAttackDamage)
			DEFINE_MEMBER_N(char* ChampionName, Offsets::GameObject::ChampionName)
			DEFINE_MEMBER_N(SpellData* MissileSpellInfo, Offsets::GameObject::MissileSpellInfo)
			DEFINE_MEMBER_N(int				MissileSrcInx, Offsets::GameObject::MissileSrcIndex)
			DEFINE_MEMBER_N(Vector3			MissileStartPos, Offsets::GameObject::MissileStartPos)
			DEFINE_MEMBER_N(Vector3			MissileEndPos, Offsets::GameObject::MissileEndPos)
			DEFINE_MEMBER_N(int				MissileDestIdx, Offsets::GameObject::MissileDestIndex)
			DEFINE_MEMBER_N(SpellBook		SpellBook, Offsets::GameObject::SpellBook)
			DEFINE_MEMBER_N(CharacterData* BaseCharacterData, Offsets::GameObject::BaseCharacterData)
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
