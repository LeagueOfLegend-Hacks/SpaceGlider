#include "BuffManager.h"
#include "../Patchables/Structs.h"

ULONG BuffScript::GetBuffHash()
{
    return *reinterpret_cast<ULONG*>(reinterpret_cast<ULONG>(this + static_cast<int>(Offsets::BuffManager::BuffHash)));
}

BuffType BuffEntry::GetType()
{
    return *reinterpret_cast<BuffType*>(reinterpret_cast<ULONG>(this + static_cast<int>(Offsets::BuffManager::BuffType)));
}

BuffScript* BuffEntry::GetBuffScript()
{
    return *reinterpret_cast<BuffScript**>(reinterpret_cast<ULONG>(this + static_cast<int>(Offsets::BuffManager::BuffScript)));
}

const char* BuffEntry::GetBuffName()
{
    const ULONG pBuffScript = *reinterpret_cast<ULONG*>(reinterpret_cast<ULONG>(this) + 0x8);
    if (!(pBuffScript > 0x1000))
        return xorstr("invalidBuff");
    return reinterpret_cast<char*>(pBuffScript + static_cast<int>(Offsets::BuffManager::BuffName));
}

float BuffEntry::GetStartTime()
{
    return *reinterpret_cast<float*>(reinterpret_cast<ULONG>(this) + static_cast<int>(Offsets::BuffManager::StartTime));
}

float BuffEntry::GetEndTime()
{
    return *reinterpret_cast<float*>(reinterpret_cast<ULONG>(this) + static_cast<int>(Offsets::BuffManager::EndTime));
}

int BuffEntry::GetBuffStacksAlt()
{
    return (*reinterpret_cast<int*>(reinterpret_cast<ULONG>(this) + static_cast<int>(Offsets::BuffManager::BuffCountAlt2)) - *reinterpret_cast<int*>(reinterpret_cast<ULONG>(this) + static_cast<int>(Offsets::BuffManager::BuffCountAlt))) >> 3;
}

bool BuffEntry::IsValid()
{
    if (reinterpret_cast<ULONG>(this) == 0 || !(reinterpret_cast<ULONG>(this) > 0x1000))
        return false;
    return strcmp(GetBuffName(), xorstr("invalidBuff")) != 0 && GetBuffStacksAlt() > 0;
}

bool BuffManager::HasBuff(char* name)
{
    for (BuffEntry* entry : GetAllBuffs())
    {
        if (strcmp(entry->GetBuffName(), name) == 0)
            return true;
    }
    return false;
}

std::vector<BuffEntry*> BuffManager::GetAllBuffs()
{
    std::vector<BuffEntry*> allBuffs;

    for (BuffEntry* buffEntry : GetBuffEntries())
    {
        if (!buffEntry || static_cast<int>(buffEntry->GetType()) == -1)
            continue;
        if (buffEntry->GetStartTime() == buffEntry->GetEndTime())
            continue;

        allBuffs.push_back(buffEntry);
    }

    return allBuffs;
}

std::vector<BuffEntry*> BuffManager::GetBuffEntries()
{
    const ULONG pBuffVectorStart{ *reinterpret_cast<ULONG*>(this + static_cast<int>(Offsets::BuffManager::ArrayStart)) };
    const ULONG pBuffVectorEnd{ *reinterpret_cast<ULONG*>(this + static_cast<int>(Offsets::BuffManager::ArrayEnd)) };
    const ULONG vectorSize{ (pBuffVectorEnd - pBuffVectorStart) / sizeof(ULONG) };

    std::vector<BuffEntry*> buffs;

    for (ULONG i = 0; i < vectorSize; i++)
    {
        BuffEntry* buff{ reinterpret_cast<BuffEntry*>(*reinterpret_cast<ULONG*>(pBuffVectorStart + 0x4 * i)) };
        if (buff && buff->IsValid())
            buffs.push_back(buff);
    }

    return buffs;

}