#pragma once
#include "../XorStr.h"
#include "../Patchables/Offsets.h"
#include "../Utils/Utils.h"

enum class BuffType {
    Internal = 0,
    Aura = 1,
    CombatEnchancer = 2,
    CombatDehancer = 3,
    SpellShield = 4,
    Stun = 5,
    Invisibility = 6,
    Silence = 7,
    Taunt = 8,
    Berserk = 9,
    Polymorph = 10,
    Slow = 11,
    Snare = 12,
    Damage = 13,
    Heal = 14,
    Haste = 15,
    SpellImmunity = 16,
    PhysicalImmunity = 17,
    Invulnerability = 18,
    AttackSpeedSlow = 19,
    NearSight = 20,
    Fear = 22,
    Charm = 23,
    Poison = 24,
    Suppression = 25,
    Blind = 26,
    Counter = 27,
    Currency = 21,
    Shred = 28,
    Flee = 29,
    Knockup = 30,
    Knockback = 31,
    Disarm = 32,
    Grounded = 33,
    Drowsy = 34,
    Asleep = 35,
    Obscured = 36,
    ClickProofToEnemies = 37,
    Unkillable = 38
};

class BuffScript
{
public:
    ULONG buffHash();
};

class BuffEntry {
public:
    union
    {
        DEFINE_MEMBER_N(BuffScript* buffScript, Offsets::BuffManager::BuffScript)
            DEFINE_MEMBER_N(BuffType type, Offsets::BuffManager::BuffType)
            DEFINE_MEMBER_N(float startTime, Offsets::BuffManager::StartTime)
            DEFINE_MEMBER_N(float endTime, Offsets::BuffManager::EndTime)
            DEFINE_MEMBER_N(int count, Offsets::BuffManager::BuffCount)
            DEFINE_MEMBER_N(int countAlt, Offsets::BuffManager::BuffCountAlt)
            DEFINE_MEMBER_N(int countAlt2, Offsets::BuffManager::BuffCountAlt2)
    };
    std::string name();
    int stacksAlt();
};

class BuffManager {
public:
    std::vector<BuffEntry*> entries();
    bool hasBuff(std::string name);
private:
    static bool buffStrainer(BuffEntry* buff);
};