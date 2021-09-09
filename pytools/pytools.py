from idc import *
from idautils import *

def DecToHex(Addr):
	return "0%0.2X" % Addr

def MakeEnum(enumName, offsetArray):
    print ("enum class %s {" % enumName)
    for offset in offsetArray:
        if len(offset[0]) == 0:
            print ("")
            continue
        if type(offset[1]) is str:
            print ("   %s = %s," % ( offset[0], offset[1]))
            continue

        fncValue = offset[1] if offset[1] != -1 else 0x0

        locByName = idc.get_name_ea_simple(offset[0])
        isMismatch = locByName == 0x0

        if locByName == BADADDR:
            locByName = fncValue
        
        if locByName > idaapi.get_imagebase():
            set_name(locByName, offset[0])
        
        if locByName > idaapi.get_imagebase():
            print ("   %s = %s,%s" % (offset[0], hex(locByName-idaapi.get_imagebase()), '// Possible mismatch' if isMismatch else ''))
        else:
            print ("   %s = %s,%s" % (offset[0], hex(locByName), '// Possible mismatch' if isMismatch else ''))

    print ("};")
    
def FindFunctionAddr(name, offset, operandValue):
    address = idc.find_binary(0, SEARCH_DOWN, "\"" + name + "\"")
    dword = -1
    
    if address == BADADDR:
        return BADADDR
    
    xrefs = XrefsTo(address)
    for xref in xrefs:
        dword = xref.frm + offset
    
    if dword == BADADDR:
        return BADADDR
    
    return idc.get_operand_value(dword, operandValue)

def FindOffsetPattern(Pattern, Operand):
	addr = idc.find_binary(0, SEARCH_DOWN, Pattern)
	if addr == BADADDR: return 0
	
	return idc.get_operand_value(addr, Operand)

def FindFunctionFirstXRef(name):
    address = idc.find_binary(0, SEARCH_DOWN, "\"" + name + "\"")
    dword = BADADDR
    
    if address == BADADDR:
        return BADADDR
    
    xrefs = XrefsTo(address)
    for xref in xrefs:
        dword = xref.frm
        break
    
    try:
        return idaapi.get_func(dword).start_ea
    except Exception:
        return -1

def FindFunctionByPatternStartEA(pattern):
    address = idc.find_binary(0, SEARCH_DOWN, pattern)
    if address == BADADDR:
        return BADADDR
    

    try:
        return idaapi.get_func(address).start_ea
    except Exception:
        return -1

def FindOffset(name, offset, operandValue):
    address = idc.find_binary(0, SEARCH_DOWN, "\"" + name + "\"")
    dword = BADADDR
    if address == BADADDR:
        return BADADDR
    xrefs = XrefsTo(address)
    for xref in xrefs:
        dword = xref.frm + offset
        break;
    if dword == BADADDR:
        return BADADDR
    return idc.get_operand_value(dword, operandValue)
    
def FindFuncCall(Pattern):
    addr = idc.find_binary(0, SEARCH_DOWN, Pattern)
    if addr == BADADDR: return 0
    return idc.get_operand_value(addr, 0)

def GetCharacterIntermediate():
    xrefs = XrefsTo(FindFunctionFirstXRef("mPrimaryARRegenRateRep"))
    dword = BADADDR
    for xref in xrefs:
        dword = xref.frm
        break
    return idc.get_operand_value(dword-0xB, 1);

def main():
    MakeEnum("Functions", [
        ["SetPlayerStats", FindFunctionFirstXRef("mPhysicalDamagePercentageModifier")],
        ["GetVersion", FindFuncCall("E8 ? ? ? ? 89 44 24 04 8B 86 ? ? ? ?")],
        ["HealthBarPosition", FindFuncCall("E8 ?? ?? ?? ?? 8B 4E ?? 8D 54 ?? ?? 52 8B 01 FF ?? ?? 5E 83 ?? ?? C3")],
        ["SetBasePlayerStats", FindFunctionFirstXRef("mIncomingHealingEnemy")],
        ["SetCharacterIntermediate", FindFunctionFirstXRef("mPrimaryARRegenRateRep")],
        ["SetCharacterIntermediate2", FindFunctionFirstXRef("mPercentBonusArmorPenetration")],
        ["SetCharacterIntermediate3", FindFunctionFirstXRef("mFlatBubbleRadiusMod")],
        ["WorldToScreen", FindFunctionByPatternStartEA("83 EC 10 56 E8 ? ? ? ? 8B 08")],
        ["GetNetClient", FindFunctionByPatternStartEA("A1 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC 83 EC 58")],
        ["GetFirstObject", FindFuncCall("E8 ? ? ? ? 8B F0 85 F6 74 1E 66 90")],
        ["GetNextObject", FindFunctionByPatternStartEA("8B 44 24 04 56 8B 71 18")],
        ["GetAttackDelay", FindFuncCall("E8 ? ? ? ? D8 44 24 14 83 C4 04")],
        ["GetAttackCastDelay", FindFuncCall("E8 ? ? ? ? D9 9E ? ? ? ? 57")],
        ["GetSpellState", FindFunctionByPatternStartEA("83 EC 14 8B 44 24 1C 55")],
        ["GetBoundingRadius", FindFuncCall("E8 ? ? ? ? D8 44 24 08 8B 7C 24 14")],
        ["BaseDrawPosition", FindFuncCall("E8 ? ? ? ? EB 07 8B 01 8B 40 28")],
        ["DrawCircle", FindFuncCall("E8 ? ? ? ? 83 C4 1C 80 7F 35 00")],
        ["IsTargetable", FindFunctionByPatternStartEA("56 8B F1 E8 ? ? ? ? 84 C0 74 2E")],
        ["IsAlive", FindFuncCall("E8 ? ? ? ? 84 C0 74 2E 8D 8E ? ? ? ?")],
        ["IsHero", FindFuncCall("E8 ? ? ? ? 83 C4 04 84 C0 74 12")],
        ["IsMissile", FindFuncCall("E8 ? ? ? ? 83 C4 04 84 C0 74 60")],
        ["IsMinion", FindFuncCall("E8 ? ? ? ? 83 C4 04 80 7F 2A 06")],
        ["IsTurret", FindFuncCall("E8 ? ? ? ? 83 C4 04 84 C0 75 38")],
        ["IsInhib", FindFuncCall("E8 ? ? ? ? 55 88 44 24 1F")],
        ["IsDragon", FindFunctionByPatternStartEA("83 EC 10 A1 ? ? ? ? 33 C4 89 44 24 0C 56 E8")],
        ["IsBaron", FindFunctionByPatternStartEA("56 E8 ? ? ? ? 68 ? ? ? ? 8B 30")],
        ["IsNexus", FindFuncCall("E8 ? ? ? ? 55 88 44 24 20")],
        ["IsNotWall", FindFuncCall("E8 ? ? ? ? 33 C9 83 C4 0C 84")],
        ["IsAI", FindFuncCall("E8 ? ? ? ? 8B 6C 24 50 83 C4 04")],
        ["IsAITurret", FindFuncCall("E8 ? ? ? ? 83 C4 04 84 C0 74 5F")],
        ["CompareObjectTypeFlags", FindFuncCall("E8 ? ? ? ? 84 C0 74 79 56")],
        ["onProcessSpell", FindFuncCall("E8 ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 80 BE ?? ?? ?? ?? ?? D8")],
        ["OnCreateObject", FindFuncCall("E8 ?? ?? ?? ?? E8 ?? ?? ?? ?? 84 C0 74 ?? 32 C9 EB")],
        ["OnDeleteObject", FindFuncCall("E8 ?? ?? ?? ?? 57 E8 ?? ?? ?? ?? 83 ?? ?? 5F 5E C3")],
        ["OnNewPath", FindFunctionByPatternStartEA("83 EC 18 56 8B 74 24 20 8B CE 57")],
    ])
    MakeEnum("Offsets", [
        ["GameTime", idc.get_operand_value(FindFunctionByPatternStartEA("E8 ? ? ? ? 8B 0D ? ? ? ? 8B D8 8B 35 ? ? ? ?")+0x7F, 1)],
        ["GameInfo", idc.get_operand_value(FindFunctionByPatternStartEA("E8 ? ? ? ? A1 ? ? ? ? 5F 5D")+0xCA, 1)],
        ["Renderer", idc.get_operand_value(FindFunctionFirstXRef("DISABLE_FOW")+0x57, 1)],
        ["ZoomClass", idc.get_operand_value(FindFunctionByPatternStartEA("A1 ? ? ? ? 89 47 60")+0xE8, 1)],
        ["ViewMatrix", idc.get_operand_value(FindFunctionByPatternStartEA("B9 ? ? ? ? E8 ? ? ? ? B9 ? ? ? ? E9 ? ? ? ? CC CC CC CC CC CC CC CC CC CC CC CC B9 ? ? ? ? B8 ? ? ? ?"), 1)],
        ["NetClient", idc.get_operand_value(FindFunctionByPatternStartEA("A1 ? ? ? ? C3 CC CC CC CC CC CC CC CC CC CC 83 EC 58"), 1)],
        ["UnderMouseObject", idc.get_operand_value(FindFunctionFirstXRef("standalone_postgame_SequenceText_%u")+0xA, 1)],   #8B 35 ? ? ? ? 8B 44 24 10
        ["Minimap", idc.get_operand_value(FindFunctionByPatternStartEA("A3 ? ? ? ? 89 35 ? ? ? ? 85 DB")+0x2B, 0)],
        ["MinimapObject", idc.get_operand_value(FindFunctionByPatternStartEA("83 3D ? ? ? ? ? 0F 84 ? ? ? ? 53 8B 5C 24 08"), 0)],
        ["HudInstance", idc.get_operand_value(FindFunctionByPatternStartEA("8B 0D ? ? ? ? FF 77 20")+0x15A, 1)],
        ["LocalPlayer", idc.get_operand_value(FindFunctionFirstXRef("Node <%s> tried to get Player champion ")+0xE, 1)],
        ["MissleList", idc.get_operand_value(FindFunctionByPatternStartEA("53 56 57 8B F9 C7 07 ? ? ? ? C7 47 ? ? ? ? ? E8 ? ? ? ?")+0x127, 1)],
        ["MinionList", idc.get_operand_value(FindFunctionByPatternStartEA("53 56 57 8B F9 C7 07 ? ? ? ? C7 47 ? ? ? ? ? E8 ? ? ? ?")+0x137, 1)],
        ["HeroList", idc.get_operand_value(FindFunctionByPatternStartEA("53 56 57 8B F9 C7 07 ? ? ? ? C7 47 ? ? ? ? ? E8 ? ? ? ?")+0x227, 1)],
        ["ObjectManager", idc.get_operand_value(FindFunctionByPatternStartEA("53 56 57 8B F9 C7 07 ? ? ? ? C7 47 ? ? ? ? ? E8 ? ? ? ?")+0x267, 1)],
        ["Chat", idc.get_operand_value(FindFunctionFirstXRef("hud_Chat_Info_ChatDisabled")+0x37, 1)],
    ])
    MakeEnum("GameObject", [
        ["Index", idc.get_operand_value(FindFunctionAddr("GetID", -0x14, 1)+0x11, 1)],
        ["TeamID", idc.get_operand_value(FindFunctionAddr("GetTeamID", -0x14, 1)+0x11, 1)],
        ["Position", FindOffsetPattern("8D 86 ? ? ? ? 8B 74 24 08", 1)],
        ["SpellBook", FindOffset("mReplicatedSpellCanCastBitsLower1",0x11,1)],
        ["mPAR", FindOffset("mMaxPAR",-0xC,1)-0x10],
        ["mMaxPAR", FindOffset("mMaxPAR",-0xC,1)],
        ["mPAREnabled", FindOffset("mPAREnabled",-0xC,1)],
        ["mPARState", FindOffset("mPARState",-0xC,1)],
        ["mSAR", FindOffset("mMaxSAR",-0xC,1)-0x10],
        ["mMaxSAR", FindOffset("mMaxSAR",-0xC,1)],
        ["mSAREnabled", FindOffset("mSAREnabled",-0xC,1)],
        ["LargePipBitField", FindOffset("LargePipBitField",-0xC,1)],
        ["MediumPipBitField", FindOffset("MediumPipBitField",-0xC,1)],
        ["mSARState", FindOffset("mSARState",-0xC,1)],
        ["mLifetime", FindOffset("mLifetime",-0xC,1)],
        ["mLifetimeTicks", FindOffset("mLifetimeTicks",-0xC,1)],
        ["PhysicalDamagePercentageModifier", FindOffset("PhysicalDamagePercentageModifier",-0xC,1)],
        ["mIsTargetable", FindOffset("mIsTargetable",-0xC,1)],
        ["mIsTargetableToTeamFlags", FindOffset("mIsTargetableToTeamFlags",-0xC,1)],
        ["HP", FindOffset("mMaxHP",0x5,1)-0x10],
        ["MaxHP", FindOffset("mMaxHP",-0x5,1)],
        ["HPMaxPenalty", FindOffset("mHPMaxPenalty",-0xC,1)],
        ["AllShield", FindOffset("mAllShield",-0xC,1)],
        ["PhysicalShield", FindOffset("mPhysicalShield",-0xC,1)],
        ["MagicalShield", FindOffset("mMagicalShield",-0xC,1)],
        ["ChampSpecificHealth", FindOffset("mChampSpecificHealth",-0xC,1)],
        ["IncomingHealingAllied", FindOffset("mIncomingHealingAllied",-0xC,1)],
        ["IncomingHealingEnemy", FindOffset("mIncomingHealingEnemy",-0xC,1)],
        ["StopShieldFade", FindOffset("mStopShieldFade",-0xC,1)],
        ["ActionState", FindOffset("mActionState",-0xC,1)],
        ["ActionState2", FindOffset("mActionState2",-0xC,1)],
        ["PercentCooldownMod", GetCharacterIntermediate()],
        ["AbilityHasteMod", GetCharacterIntermediate()+FindOffset("mAbilityHasteMod",0x5,1)],
        ["PercentCooldownCapMod", GetCharacterIntermediate()+FindOffset("mPercentCooldownCapMod",0x5,1)],
        ["PassiveCooldownEndTime", GetCharacterIntermediate()+FindOffset("mPassiveCooldownEndTime",-0x3,1)],
        ["PassiveCooldownTotalTime", GetCharacterIntermediate()+FindOffset("mPassiveCooldownTotalTime",0x5,1)],
        ["FlatPhysicalDamageMod", GetCharacterIntermediate()+0xFFFFFF00-FindOffset("mFlatPhysicalDamageMod",0x5,1)], # this is weird in asm, its the only one we need to subtract this odd offset to get the correct offset.
        ["PercentPhysicalDamageMod", GetCharacterIntermediate()+FindOffset("mPercentPhysicalDamageMod",0x5,1)],
        ["PercentBonusPhysicalDamageMod", GetCharacterIntermediate()+ FindOffset("mPercentBonusPhysicalDamageMod",0x5,1)],
        ["PercentBasePhysicalDamageAsFlatBonusMod", GetCharacterIntermediate()+ FindOffset("mPercentBasePhysicalDamageAsFlatBonusMo",0x5,1)],
        ["FlatMagicDamageMod", GetCharacterIntermediate()+ FindOffset("mFlatMagicDamageMod",0x5,1)],
        ["PercentMagicDamageMod", GetCharacterIntermediate()+ FindOffset("mPercentMagicDamageMod",0x5,1)],
        ["FlatMagicReduction", GetCharacterIntermediate()+ FindOffset("mFlatMagicReduction",0x5,1)],
        ["PercentMagicReduction", GetCharacterIntermediate()+ FindOffset("mPercentMagicReduction",0x5,1)],
        ["FlatCastRangeMod", GetCharacterIntermediate() + FindOffset("mFlatCastRangeMod",0x5,1)],
        ["AttackSpeedMod", GetCharacterIntermediate()+ FindOffset("mAttackSpeedMod",0x5,1)],
        ["PercentAttackSpeedMod", GetCharacterIntermediate()+ FindOffset("mPercentAttackSpeedMod",0x5,1)],
        ["PercentMultiplicativeAttackSpeedMod", GetCharacterIntermediate()+ FindOffset("mPercentMultiplicativeAttackSpeedMod",0x5,1)],
        ["BaseAttackDamage", GetCharacterIntermediate()+ FindOffset("mBaseAttackDamage",0x5,1)],
        ["BaseAttackDamageSansPercentScale", GetCharacterIntermediate()+ FindOffset("mBaseAttackDamageSansPercentScale",0x5,1)],
        ["FlatBaseAttackDamageMod", GetCharacterIntermediate()+ FindOffset("mFlatBaseAttackDamageMod",0x5,1)],
        ["PercentBaseAttackDamageMod", GetCharacterIntermediate()+ FindOffset("mPercentBaseAttackDamageMod",0x5,1)],
        ["BaseAbilityDamage", GetCharacterIntermediate()+ FindOffset("mBaseAbilityDamage",0x5,1)],
        ["CritDamageMultiplier", GetCharacterIntermediate()+ FindOffset("mCritDamageMultiplier",0x5,1)],
        ["ScaleSkinCoef", GetCharacterIntermediate()+ FindOffset("mScaleSkinCoef",0x5,1)],
        ["Dodge", GetCharacterIntermediate()+ FindOffset("mDodge",0x5,1)],
        ["Crit", GetCharacterIntermediate()+ FindOffset("mCrit",0x5,1)],
        ["FlatBaseHPPoolMod", GetCharacterIntermediate()+ FindOffset("mFlatBaseHPPoolMod",0x5,1)],
        ["Armor", GetCharacterIntermediate()+ FindOffset("mArmor",0x5,1)],
        ["BonusArmor", GetCharacterIntermediate()+ FindOffset("mBonusArmor",0x5,1)],
        ["SpellBlock", GetCharacterIntermediate()+ FindOffset("mSpellBlock",0x5,1)],
        ["BonusSpellBlock", GetCharacterIntermediate()+ FindOffset("mBonusSpellBlock",0x5,1)],
        ["HPRegenRate", GetCharacterIntermediate()+ FindOffset("mHPRegenRate",0x5,1)],
        ["BaseHPRegenRate", GetCharacterIntermediate()+ FindOffset("mBaseHPRegenRate",0x5,1)],
        ["MoveSpeed", GetCharacterIntermediate()+ FindOffset("mMoveSpeed",0x5,1)],
        ["AttackRange", GetCharacterIntermediate()+ FindOffset("mAttackRange",0x5,1)],
        ["FlatBubbleRadiusMod", GetCharacterIntermediate()+ FindOffset("mFlatBubbleRadiusMod",0x5,1)],
        ["PercentBubbleRadiusMod", GetCharacterIntermediate()+ FindOffset("mPercentBubbleRadiusMod",0x5,1)],
        ["FlatArmorPenetration", GetCharacterIntermediate()+ FindOffset("mFlatArmorPenetration",0x5,1)],
        ["PhysicalLethality", GetCharacterIntermediate()+ FindOffset("mPhysicalLethality",0x5,1)],
        ["PercentArmorPenetration", GetCharacterIntermediate()+ FindOffset("mPercentArmorPenetration",0x5,1)],
        ["PercentBonusArmorPenetration", GetCharacterIntermediate()+ FindOffset("mPercentBonusArmorPenetration",0x5,1)],
        ["PercentCritBonusArmorPenetration", GetCharacterIntermediate()+ FindOffset("mPercentCritBonusArmorPenetration",0x5,1)],
        ["PercentCritTotalArmorPenetration", GetCharacterIntermediate()+ FindOffset("mPercentCritTotalArmorPenetration",0x5,1)],
        ["FlatMagicPenetration", GetCharacterIntermediate()+ FindOffset("mFlatMagicPenetration",0x5,1)],
        ["MagicLethality", GetCharacterIntermediate()+ FindOffset("mMagicLethality",0x5,1)],
        ["PercentMagicPenetration", GetCharacterIntermediate()+ FindOffset("mPercentMagicPenetration",0x5,1)],
        ["PercentBonusMagicPenetration", GetCharacterIntermediate()+ FindOffset("mPercentBonusMagicPenetration",0x5,1)],
        ["PercentLifeStealMod", GetCharacterIntermediate()+ FindOffset("mPercentLifeStealMod",0x5,1)],
        ["PercentSpellVampMod", GetCharacterIntermediate()+ FindOffset("mPercentSpellVampMod",0x5,1)],
        ["PercentOmnivampMod", GetCharacterIntermediate()+ FindOffset("mPercentOmnivampMod",0x5,1)],
        ["PercentPhysicalVamp", GetCharacterIntermediate()+ FindOffset("mPercentPhysicalVamp",0x5,1)],
        ["PathfindingRadiusMod", GetCharacterIntermediate()+ FindOffset("mPathfindingRadiusMod",0x5,1)],
        ["PercentCCReduction", GetCharacterIntermediate()+ FindOffset("mPercentCCReduction",0x5,1)],
        ["PercentEXPBonus", GetCharacterIntermediate()+ FindOffset("mPercentEXPBonus",0x5,1)],
        ["PARRegenRate", GetCharacterIntermediate()+ FindOffset("mPARRegenRate",-0x6,1)],
        ["PrimaryARBaseRegenRateRep", GetCharacterIntermediate()+ FindOffset("mPrimaryARBaseRegenRateRep",-0x6,1)],
        ["SecondaryARRegenRateRep", GetCharacterIntermediate()+ FindOffset("mSecondaryARRegenRateRep",0x5,1)],
        ["SecondaryARBaseRegenRateRep", GetCharacterIntermediate()+ FindOffset("mSecondaryARBaseRegenRateRep",0x5,1)],
        ["Gold", FindOffset("mGold",0x5,1)],
        ["GoldTotal", FindOffset("mGoldTotal",0x5,1)],
        ["MinimumGold", FindOffset("mGoldTotal",-0xC,1)],
        ["CombatType", FindOffset("mCombatType",-0xC,1)],
        ["Exp", FindOffset("mExp",-0x6,1)],
        ["mLevelRef", FindOffset("mLevelRef",-0x10,1)],
        ["SkillUpLevelDeltaReplicate", FindOffset("mInputLocks",-0x13,1)],
        ["VisionScore", FindOffset("mInputLocks",-0xE,1)],
        ["mNumNeutralMinionsKilled", FindOffset("mNumNeutralMinionsKilled",-0xC,1)],
        ["InputLocks", FindOffset("mInputLocks",-0xC,1)],
        ["ShutdownValue", FindOffset("mShutdownValue",-0xC,1)],
        ["BaseGoldGivenOnDeath", FindOffset("mBaseGoldGivenOnDeath",-0xC,1)],
    ])
main()
