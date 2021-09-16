from idc import BADADDR, INF_BASEADDR, SEARCH_DOWN, FUNCATTR_START, FUNCATTR_END
import idc
import idaapi
import datetime

def MakeEnum(enumName, offsetArray):
    print ("enum class %s\r\n{" % enumName)
    for offset in offsetArray:
        if len(offset[0]) == 0:
            print ("")
            continue
        if type(offset[1]) is str:
            print ("   %s = %s," % ( offset[0], offset[1]))
            continue

        fncValue = offset[1] if offset[1] != -1 else 0x0

        isMismatch = fncValue == BADADDR or fncValue == 0
        
        if (fncValue >= idaapi.get_imagebase() and not isMismatch):
            idc.set_name(fncValue, offset[0])
            fncValue = fncValue - idaapi.get_imagebase()


        print ("   %s = 0x%x,%s" % (offset[0], fncValue, '// Possible mismatch' if isMismatch else ''))

    print ("};\r\n")

def FindOffsetByString(name, offset, operandValue):
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

def FindFuncCall(Pattern):
    addr = idc.find_binary(0, SEARCH_DOWN, Pattern)
    if addr == BADADDR: return 0
    return idc.get_operand_value(addr, 0)

def FindFunctionByPatternStartEA(pattern):
	address = idc.find_binary(0, SEARCH_DOWN, pattern)
	if address == BADADDR:
		return BADADDR

	try:
		return idaapi.get_func(address).start_ea
	except Exception:
		return -1

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
	
	try:
		return idaapi.get_func(dword).start_ea
	except Exception:
		return -1

def main():	
    print("[*] League of Legends Client Update Tool")
    print("[*] By Dencelle")
    print("[*] Started at: %s" % datetime.datetime.now())
    print("----------------------------")
    MakeEnum("Functions", [
        ["Logger", FindFunctionByPatternStartEA("8D 44 24 14 50 FF 74 24 14 FF")],
        ["LogUIEvents", FindFunctionByPatternStartEA("53 55 56 8B F1 57 8B 7C 24 18")],
        ["SendChat", FindFunctionByPatternStartEA("56 6A FF FF 74")],
        ["DrawTurretRange", FindFuncCall("E8 ? ? ? ? 84 C0 75 1B 83")],
        ["Hud_OnDisconnect", FindFunctionFirstXRef("game_messagebox_caption_disconnect")],
        ["Hud_OnAfk", FindFunctionFirstXRef("game_messagebox_text_afkwarningcaption")],
        ["pwOpen", FindFunctionFirstXRef("game_console_chatcommand_allchat_1")],
        ["r3dProjectToScreen", FindFunctionByPatternStartEA("0f 2f 05 ?? ?? ?? ?? 73 ?? 0f 2f da 77")],
    ])
    MakeEnum("BuffHost", [
        ["BuffAddRemove", FindFunctionFirstXRef("SpellToggleSlot")]
    ])
    MakeEnum("AudioManager", [
        ["PlaySound", FindFunctionFirstXRef("AudioManager::PlaySoundEvent: Failed to play sound event %s.")]
    ])
    MakeEnum("Game", [
        ["IsNotWall", FindFuncCall("E8 ? ? ? ? 33 C9 83 C4 0C 84")],
        ["GameVersion", FindFunctionByPatternStartEA("8B 44 24 04 BA ? ? ? ? 2B D0")],
        ["", 0x0],
        ["GameInfo", FindOffsetPattern("A1 ? ? ? ? 83 78 08 02 0F 94", 1)],
        ["GameTime", FindOffsetPattern("F3 0F 11 05 ? ? ? ? 5E 83 C4 08 C2 04 00", 0)],
        ["", 0x0],
        ["NetClient", FindOffsetPattern("8B 0D ? ? ? ? 85 C9 74 07 8B 01 6A 01 FF 50 08 8B", 1)],
        ["OnProcessSpell", FindFuncCall("E8 ?? ?? ?? ?? 8B CE E8 ?? ?? ?? ?? 80 BE ?? ?? ?? ?? ?? D8")],
    ])
    MakeEnum("Render", [
        ["ViewMatrix", FindOffsetPattern("8D 4A ? 51 81 C2 ? ? ? ? 52", 1)],
        ["Projection", FindOffsetPattern("81 C2 ? ? ? ? 52 FF 74 24 38", 1)],
        ["ZoomClass", FindOffsetPattern("A3 ? ? ? ? 83 FA 10 72 32", 0)],
        ["Renderer", FindOffsetPattern("8B 15 ? ? ? ? 83 EC 08 F3", 1)],
        ["HudInstance", FindOffsetPattern("8B 0D ? ? ? ? 6A 00 8B 49 34 E8 ? ? ? ? B0", 1)], 
        ["DrawCircle", FindFuncCall("E8 ? ? ? ? 83 C4 1C 80 7F")],
        ["WorldToScreen", FindFunctionByPatternStartEA("83 EC 10 56 E8 ? ? ? ? 8B 08")],
    ])
    MakeEnum("ObjectManager", [
        ["GetFirstObject", FindFuncCall("E8 ? ? ? ? 8B F0 85 F6 74 1E 66 90")],
        ["GetNextObject", FindFunctionByPatternStartEA("8B 44 24 04 56 8B 71 18")],
        ["ObjManager", FindOffsetPattern("8B 0D ? ? ? ? E8 ? ? ? ? FF 77", 1)],
        ["LocalPlayer", FindOffsetPattern("A1 ? ? ? ? 85 C0 74 07 05 ? ? ? ? EB 02 33 C0 56", 1)],
        ["ManagerTemplate_Minions", FindOffsetPattern("A1 ?? ?? ?? ?? 53 55 8B 6C 24 1C", 1)],
        ["ManagerTemplate_Heros", FindOffsetPattern("8B 0D ?? ?? ?? ?? 50 8D 44 24 18", 1)],
        ["ManagerTemplate_Turrets", FindOffsetPattern("A1 ? ? ? ? 53 56 8B 70 04", 1)],
        ["ManagerTemplate_Missiles", FindOffsetPattern("8B 1D ? ? ? ? 85 DB 0F 84 ? ? ? ? 0F B6 43 08 33 C9", 1)],
    ])
    MakeEnum("GameObjectFunctions", [
        ["GetAttackDelay", FindFunctionByPatternStartEA("8B 44 24 04 51 F3")],
        ["GetAttackCastDelay", FindFunctionByPatternStartEA("83 EC 0C 53 8B 5C 24 14 8B CB 56 57 8B 03 FF 90")],
        ["IsAlive", FindFunctionByPatternStartEA("56 8B F1 8B 06 8B 80 ? ? ? ? FF D0 84 C0 74 19")],
        ["IsInhib", FindFuncCall("E8 ? ? ? ? 83 C4 04 84 C0 74 38")],
        ["IsNexus", FindFuncCall("E8 ? ? ? ? 55 88 44 24 20")],
        ["IsTurret", FindFuncCall("E8 ? ? ? ? 83 C4 04 84 C0 74 09 5F")],
        ["IsMinion", FindFuncCall("E8 ? ? ? ? 83 C4 04 84 C0 74 16 8B 0F")],
        ["IsDragon", FindFuncCall("E8 ? ? ? ? 33 DB 84 C0 0F 95 C3 8D 1C 5D ? ? ? ? E9")],
        ["IsBaron", FindFuncCall("E8 ? ? ? ? 84 C0 74 0A BB")],
        ["IsHero", FindFuncCall("E8 ? ? ? ? 83 C4 04 84 C0 74 2B")],
        ["IsMissile", FindFuncCall("E8 ? ? ? ? 83 C4 04 84 C0 74 60")],
        ["CompareObjectTypeFlags", FindFuncCall("E8 ? ? ? ? 84 C0 74 79 56")],
        ["GetSpellState", FindFuncCall("E8 ? ? ? ? 8B F8 8B CB 89")],
        ["GetBasicAttack", FindFunctionByPatternStartEA("53 8B D9 B8 ? ? ? ? 8B 93")],
    ])
    MakeEnum("GameObject", [
        ["ID", FindOffsetPattern("8B 40 20 C3 33 C0 C3 CC CC", 1)],
        ["TeamID", FindOffsetPattern("8B 40 4C C3 33 C0", 1)],
        ["Name", FindOffsetPattern("8D 4E 6C E8 ? ? ? ? 8D 4E 60 E8 ? ? ? ? 8D 4E 54 E8 ? ? ? ? 8D 4E 48 E8 ? ? ? ?", 1)],
        ["NetworkID", FindOffsetPattern("8D 83 ? ? ? ? 50 8D 44 24 34 50 E8 ? ? ? ? 8B 00 89 83 ? ? ? ? 8B 8B ? ? ? ?", 1)],
        ["Position", FindOffsetPattern("8D 86 ? ? ? ? 8B 74 24 08", 1)],
        ["Mana", FindOffsetPattern("8D B3 ? ? ? ? F3 0F 11 83 ? ? ? ? BF ? ? ? ? C6 83 ? ? ? ? ?", 1)],
        ["MaxMana", FindOffsetPattern("8D B3 ? ? ? ? F3 0F 11 83 ? ? ? ? BF ? ? ? ? C6 83 ? ? ? ? ?", 1)+0x10],
        ["Health", FindOffsetPattern("8D 8B ? ? ? ? C6 83 ? ? ? ? ? 50 8D 83 ? ? ? ? C7 83 ? ? ? ? ? ? ? ? 50", 1)],
        ["MaxHealth", FindOffsetByString("mMaxHP",0x5,1)],
        ["AttackRange", FindOffsetPattern("D8 81 ? ? ? ? 8B 81 ? ? ? ?", 1)],
        ["Gold", FindOffsetByString("mGold",0x5,1)],
        ["CombatType", FindOffsetByString("mCombatType",-0xC,1)],
        ["SpellBook", FindOffsetByString("mReplicatedSpellCanCastBitsLower1", 0x11, 1)],
        ["Experience", FindOffsetByString("mExp",-0x6,1)],
        ["Level", FindOffsetByString("mLevelRef",-0x10,1)],
        ["HeroInventory", FindOffsetPattern("8D 8B ? ? ? ? E8 ? ? ? ? 8D B3 ? ? ? ? C7 06 ? ? ? ? 89 5E 04 6A 18", 1)],
    ])
    print('----------------------------')
    print("[*] Finished")
main()