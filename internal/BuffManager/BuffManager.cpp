#include "BuffManager.h"

template <class Functor>
class Not
{
public:
    Not(Functor& f) : func(f) {}

    template <typename ArgType>
    bool operator()(ArgType& arg) { return !func(arg); }

private:
    Functor& func;
};

template<typename T, typename B>
T filter(T list, B pred) {
    T ret;
    std::remove_copy_if(
        list.begin(),
        list.end(),
        std::back_insert_iterator<T>(ret),
        Not<B>(pred)
    );

    return ret;
}

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
    /*if (std::find(GetAllBuffs().begin(), GetAllBuffs().end(), name) != GetAllBuffs().end()) // buff vector example
    {
        return true;
    }*/
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
    return true;
}

std::vector<BuffEntry*> BuffManager::entries()
{
    std::vector<BuffEntry*> buffs = *reinterpret_cast<std::vector<BuffEntry*>*>(this + static_cast<int>(Offsets::BuffManager::ArrayStart));

    return filter(buffs, &buffStrainer);
}