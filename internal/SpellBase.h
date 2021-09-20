#pragma once
#include "Patchables/Offsets.h"
#include "Utils/Utils.h"
#include "Enums.h"

class CSpellData
{
public:
    union
    {
        DEFINE_MEMBER_N(char* missileName, 0x6C)
        DEFINE_MEMBER_N(char* name, 0x90)
    };
    float mana(int level) {
        if (level > 4) {
            return 0.f;
        }
        return *reinterpret_cast<float*>(this + static_cast<int>(0x52C) + ((level - 1) * 0x4));
    }
};

class CSpellInfo
{
public:
    DEFINE_MEMBER_N(CSpellData* spellData, 0x44)
};

class CSpellBase
{
public:
    union
    {
        DEFINE_MEMBER_N(int level, 0x20)
            DEFINE_MEMBER_N(float lastRelease, 0x28)
            DEFINE_MEMBER_N(bool isActive, 0x74)
            DEFINE_MEMBER_N(CSpellInfo* spellInfo, 0x13C)
    };
    bool ready() {
        int time = *(float*)(DEFINE_RVA(Offsets::Data::GameTime));

        return time - this->lastRelease >= 0 && this->level > 0;
    }
    float remainingCooldown() {
        float time = *(float*)(DEFINE_RVA(Offsets::Data::GameTime));
        /*if (!this->ready())
            return 0.0f;*/
        return this->lastRelease - time;
    }
    float levelCooldown() {
        return *reinterpret_cast<float*>(this->spellInfo->spellData + 0x27C + static_cast<int>(0x4 * this->level));
    }
};

class CSpellBook
{
public:
    SpellState getSpellState(int slot) {
        bool junk;
        return reinterpret_cast<SpellState(__thiscall*)(CSpellBook*, int, bool*)>(DEFINE_RVA(0x4EB890))(this, slot, &junk);
    }
    CSpellBase* spellId(int id) {
        return *reinterpret_cast<CSpellBase**>(this + 0x478 + (0x4 * id));
    }
};
