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

def FindNOPAddr(name, offset):
	address = idc.FindBinary(0, SEARCH_DOWN, "\"" + name + "\"")
	dword = -1
	
	if address == BADADDR:
		return -1
	
	xrefs = XrefsTo(address)
	for xref in xrefs:
		dword = xref.frm + offset

	return dword

def FindFunctionAddrByPattern(displayName, pattern, offset, operandValue):
	address = idc.FindBinary(0, SEARCH_DOWN, pattern)
	if address != BADADDR:
		return BADADDR
	
	return idc.GetOperandValue(address, operandValue)

def FindFunctionAddr(name, offset, operandValue):
	address = idc.FindBinary(0, SEARCH_DOWN, "\"" + name + "\"")
	dword = -1
	
	if address == BADADDR:
		return BADADDR
	
	xrefs = XrefsTo(address)
	for xref in xrefs:
		dword = xref.frm + offset
	
	if dword == BADADDR:
		return BADADDR

	return idc.GetOperandValue(dword, operandValue)
	
def FindFunctionByPattern(pattern, offset):
	address = idc.find_binary(0, SEARCH_DOWN, pattern)
		
	return address

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

def FindOffsetPattern(Pattern, Operand):
    addr = idc.find_binary(0, SEARCH_DOWN, Pattern)
    if addr == BADADDR: return 0
    return idc.get_operand_value(addr, Operand)
    
def main():	
    print ("[*] League of Legends Client Update Tool")
    print ("[*] By Dencelle")
    print ("[*] Started at: %s" % datetime.datetime.now())
    print ("----------------------------")
    MakeEnum("Game", [
        ["IsNotWall", FindFuncCall("E8 ? ? ? ? 33 C9 83 C4 0C 84")],
        ["GameVersion", FindFunctionByPatternStartEA("8B 44 24 04 BA ? ? ? ? 2B D0")],
        ["", 0x0],
        ["GameInfo", FindOffsetPattern("A1 ? ? ? ? 83 78 08 02 0F 94", 1)],
        ["GameTime", FindOffsetPattern("F3 0F 11 05 ? ? ? ? 5E 83 C4 08 C2 04 00", 0)],
        ["", 0x0],
        ["NetClient", FindOffsetPattern("8B 0D ? ? ? ? 85 C9 74 07 8B 01 6A 01 FF 50 08 8B", 1)],
        ])
    MakeEnum("Render", [
        ["oViewMatrix", FindOffsetPattern("8D 4A ? 51 81 C2 ? ? ? ? 52", 1)],
        ["oProjection", FindOffsetPattern("81 C2 ? ? ? ? 52 FF 74 24 38", 1)],
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
        ])    
    MakeEnum("GameObject", [
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
        ["GetSpellState", FindFuncCall("E8 ? ? ? ? 8B F8 8B CB 89")],
        ["GetBasicAttack", FindFunctionByPatternStartEA("53 8B D9 B8 ? ? ? ? 8B 93")],
        ["AttackRange", FindOffsetPattern("D8 81 ? ? ? ? 8B 81 ? ? ? ?", 1)],
        ])
        
    print ('----------------------------')
    print ("[*] Finished")
	
main()