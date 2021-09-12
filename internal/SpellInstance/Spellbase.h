#include "../Patchables/Offsets.h"
#include "../Utils/Utils.h"
#include "../Enums.h"

class CSpellData
{
public:
    union
    {
        DEFINE_MEMBER_N(char* missileName, 0x6C)
        DEFINE_MEMBER_N(char* name, 0x90)
    };
    float mana(int level);
};

class CSpellInfo
{
public:
    CSpellData* spellData();
};

class CSpellBase
{
public:
    union
    {
        DEFINE_MEMBER_N(int level, 0x20)
            DEFINE_MEMBER_N(float lastRelease, 0x28)
            DEFINE_MEMBER_N(bool isActive, 0x74)
    };
    bool ready();
    float remainingCooldown();
    float levelCooldown();

    CSpellInfo* spellInfo();
};

class CSpellBook
{
public:
    SpellState getSpellState(int slot);
    CSpellBase* spellId(int id);
};
