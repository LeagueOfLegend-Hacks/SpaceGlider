#pragma once
#include "../SpellInstance/Spellbase.h"

float CSpellData::mana(int level) {
    if (level > 4) {
        return 0.f;
    }
    return *reinterpret_cast<float*>(this + static_cast<int>(0x52C) + ((level - 1) * 0x4));
}

bool CSpellBase::ready()
{
    int time = *(float*)(DEFINE_RVA(Offsets::Data::GameTime));

    return (int)this->lastRelease - (int)time <= 0 && this->level > 0;
}

float CSpellBase::remainingCooldown()
{
    float time = *(float*)(DEFINE_RVA(Offsets::Data::GameTime));
    if (!this->ready())
        return 0.0f;
    return this->lastRelease - time;
}

float CSpellBase::levelCooldown()
{
    return *reinterpret_cast<float*>(spellInfo()->spellData() + 0x27C + static_cast<int>(0x4 * this->level));
}

SpellState CSpellBook::getSpellState(int slot)
{
    bool junk;
    return reinterpret_cast<SpellState(__thiscall*)(CSpellBook*, int, bool*)>(DEFINE_RVA(0x4EB890))(this, slot, &junk);
}

CSpellInfo* CSpellBase::spellInfo()
{
    return (CSpellInfo*)*(unsigned long*)((unsigned long)this + static_cast<int>(0x13C));
}

CSpellData* CSpellInfo::spellData()
{
    return (CSpellData*)*(unsigned long*)((unsigned long)this + static_cast<int>(0x44));
}

CSpellBase* CSpellBook::spellId(int id)
{
    return *reinterpret_cast<CSpellBase**>(this + 0x478 + (0x4 * id));
}
