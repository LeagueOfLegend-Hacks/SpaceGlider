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

def main():
    MakeEnum("Functions", [
    ])
main()
