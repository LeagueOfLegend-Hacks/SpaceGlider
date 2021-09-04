#pragma once
#include "Offsets.h"
#include "Utils.h"
#include "Vector.h"
#include "Enums.h"
#include <string>
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
	};
	float GameObject::GetBoundingRadius() {
		return reinterpret_cast<float(__thiscall*)(GameObject*)>(this->VTable[35])(this);
	}
	bool IsAlive() {
		typedef bool(__thiscall* fnIsAlive)(GameObject* pObj);
		return ((fnIsAlive)(DEFINE_RVA(Offsets::Functions::IsAlive)))(this);
	}
	bool IsHero() {
		typedef bool(__cdecl* fnIsHero)(GameObject* pObj);
		return ((fnIsHero)(DEFINE_RVA(Offsets::Functions::IsHero)))(this);
	}
	bool IsMissile() {
		typedef bool(__cdecl* fnIsMissile)(GameObject* pObj);
		return ((fnIsMissile)(DEFINE_RVA(Offsets::Functions::IsMissile)))(this);
	}
	bool IsMinion() {
		typedef bool(__cdecl* fnMinion)(GameObject* pObj);
		return ((fnMinion)(DEFINE_RVA(Offsets::Functions::IsMinion)))(this);
	}
	bool IsInhibitor() {
		typedef bool(__cdecl* fnIsInhibitor)(GameObject* pObj);
		return ((fnIsInhibitor)(DEFINE_RVA(Offsets::Functions::IsInhib)))(this);
	}
	bool IsBaron() {
		typedef bool(__cdecl* fnIsBaron)(GameObject* pObj);
		return ((fnIsBaron)(DEFINE_RVA(Offsets::Functions::IsBaron)))(this);
	}
	bool IsNexus() {
		typedef bool(__cdecl* fnIsNexus)(GameObject* pObj);
		return ((fnIsNexus)(DEFINE_RVA(Offsets::Functions::IsNexus)))(this);
	}
	bool IsTurret() {
		typedef bool(__cdecl* fnIsTurret)(GameObject* pObj);
		return ((fnIsTurret)(DEFINE_RVA(Offsets::Functions::IsTurret)))(this);
	}
	float GetAttackDelay() {
		typedef float(__cdecl* fnGetAttackDelay)(GameObject* pObj);
		return ((fnGetAttackDelay)(DEFINE_RVA(Offsets::Functions::GetAttackDelay)))(this);
	}
	float GetAttackCastDelay() {
		typedef float(__cdecl* fnGetAttackCastDelay)(GameObject* pObj);
		return ((fnGetAttackCastDelay)(DEFINE_RVA(Offsets::Functions::GetAttackCastDelay)))(this);
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
	bool IsMonster() {
		return this->IsNeutral() && this->MaxHealth > 6.0f;
	}
	std::string GetChampionName() {
		return std::string(this->ChampionName);
	}
	float CalcDamage(GameObject* Target) {
		auto Damage = Target->BaseAttackDamage + Target->BonusAttackDamage;
		return Damage - (Target->Armor + Target->BonusArmor);
	}
};
template<typename T>
struct SEntityList {
	char pad[0x4];
	T** entities;
	size_t size;
	size_t max_size;
};