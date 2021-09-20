#include "BuffManager.h"

ULONG BuffScript::buffHash()
{
    return *reinterpret_cast<ULONG*>(reinterpret_cast<ULONG>(this + static_cast<int>(Offsets::BuffManager::BuffHash)));
}

std::string BuffEntry::name()
{
    const ULONG pBuffScript = *reinterpret_cast<ULONG*>(reinterpret_cast<ULONG>(this) + 0x8);
    if (!(pBuffScript > 0x1000))
        return xorstr("invalidBuff");
    auto name = reinterpret_cast<char*>(pBuffScript + static_cast<int>(Offsets::BuffManager::BuffName));
    return std::string(name);
}

int BuffEntry::stacksAlt()
{
    return (*reinterpret_cast<int*>(reinterpret_cast<ULONG>(this) + static_cast<int>(Offsets::BuffManager::BuffCountAlt2)) - *reinterpret_cast<int*>(reinterpret_cast<ULONG>(this) + static_cast<int>(Offsets::BuffManager::BuffCountAlt))) >> 3;
}

bool BuffManager::hasBuff(std::string name)
{
    for (auto b : this->entries()) {
        if (b->name() == name) {
            return true;
        }
    }
    return false;
}

bool BuffManager::buffStrainer(BuffEntry* buff)
{
    if (!buff || static_cast<int>(buff->type) == -1)
        return false;
    if (reinterpret_cast<ULONG>(buff) == 0 || !(reinterpret_cast<ULONG>(buff) > 0x1000))
        return false;
    if ((int)(*(float*)(DEFINE_RVA(Offsets::Data::GameTime))) > buff->endTime)
        return false;
    if (strcmp(buff->name().c_str(), xorstr("invalidBuff")) == 0 || buff->stacksAlt() == 0)
        return false;
    return true;
}

std::vector<BuffEntry*> BuffManager::entries()
{
    std::vector<BuffEntry*> buffs = *reinterpret_cast<std::vector<BuffEntry*>*>(this + static_cast<int>(Offsets::BuffManager::ArrayStart));

    return filterVector(buffs, buffStrainer);
}
