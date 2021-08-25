#include "Decrypt.h"

uint8_t* LeagueDecrypt::FindSignature(const LPCSTR szModule, const char* szSignature)
{
    const HMODULE module = GetModuleHandleA(szModule);
    static auto patternToByte = [](const char* pattern)
    {
        auto bytes = std::vector<int>{};
        char* start = const_cast<char*>(pattern);
        char* end = const_cast<char*>(pattern) + strlen(pattern);

        for (char* current = start; current < end; ++current)
        {
            if (*current == '?')
            {
                ++current;
                if (*current == '?')
                    ++current;
                bytes.push_back(-1);
            }
            else
                bytes.push_back(strtoul(current, &current, 16));
        }
        return bytes;
    };

    if (module)
    {
        auto* dosHeader = reinterpret_cast<PIMAGE_DOS_HEADER>(module);
        auto* ntHeaders = reinterpret_cast<PIMAGE_NT_HEADERS>(reinterpret_cast<uint8_t*>(module) + dosHeader->e_lfanew);
        auto* textSection = IMAGE_FIRST_SECTION(ntHeaders);

        const DWORD sizeOfImage = textSection->SizeOfRawData;
        auto patternBytes = patternToByte(szSignature);
        uint8_t* scanBytes = reinterpret_cast<uint8_t*>(module) + textSection->VirtualAddress;

        const size_t size = patternBytes.size();
        int* data = patternBytes.data();

        auto mbi = MEMORY_BASIC_INFORMATION{ nullptr };
        uint8_t* nextCheckAddress = nullptr;

        for (auto i = 0ul; i < sizeOfImage - size; ++i)
        {
            bool found = true;
            for (auto j = 0ul; j < size; ++j)
            {
                unsigned char* const currentAddress = scanBytes + i + j;
                if (currentAddress >= nextCheckAddress)
                {
                    if (!VirtualQuery(reinterpret_cast<void*>(currentAddress), &mbi, sizeof(mbi)))
                        break;

                    if (mbi.Protect == PAGE_NOACCESS)
                    {
                        i += reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize - (reinterpret_cast<std::uintptr_t>(scanBytes) + i);
                        i--;
                        found = false;
                        break;
                    }
                    nextCheckAddress = static_cast<uint8_t*>(mbi.BaseAddress) + mbi.RegionSize;
                }

                if (scanBytes[i + j] != data[j] && data[j] != -1)
                {
                    found = false;
                    break;
                }
            }
            if (found)
                return &scanBytes[i];
        }
    }
    return nullptr;
}
uint8_t* LeagueDecrypt::FindRtlDispatchExceptionAddress()
{
    auto* address = FindSignature("ntdll.dll", "E8 ? ? ? ? 0A C0");
    if (!address)
        return nullptr;

    address = address + *reinterpret_cast<uint32_t*>(address + 1) + 5;
    return address;
}
int LeagueDecrypt::IsMemoryDecrypted(PVOID address)
{
    CONTEXT ctx;
    EXCEPTION_RECORD exr;
    MEMORY_BASIC_INFORMATION mbi;
    FLOATING_SAVE_AREA w64;
    memset(&mbi, 0, sizeof(mbi));
    VirtualQuery(address, &mbi, sizeof(mbi));
    if (mbi.Protect != PAGE_NOACCESS)
        return 1;
    RtlCaptureContext(&ctx);
    memset(&exr, 0, sizeof(EXCEPTION_RECORD));
    memset(&w64, 0, sizeof(WOW64_FLOATING_SAVE_AREA));

#ifdef _WIN64
    ctx.Rip = reinterpret_cast<DWORD64>(Address);// (DWORD)FinishThread;
#else
    ctx.Eip = reinterpret_cast<DWORD>(address);// (DWORD)FinishThread;
#endif // 

    ctx.ContextFlags = 0x1007F;
    ctx.SegCs = 0x23;
    ctx.SegDs = 0x2B;
    ctx.SegEs = 0x2B;
    ctx.SegFs = 0x53;
    ctx.SegGs = 0x2B;
    ctx.SegSs = 0x2B;
    exr.ExceptionAddress = address;
    exr.NumberParameters = 2;
    exr.ExceptionCode = EXCEPTION_ACCESS_VIOLATION;
    exr.ExceptionInformation[1] = reinterpret_cast<DWORD>(address);
    _EXCEPTION_POINTERS ei;
    ei.ContextRecord = &ctx;
    ei.ExceptionRecord = &exr;

    if (!_RtlDispatchExceptionAddress)
        return 0;

    const DWORD RtlDispatchExceptionAddr = reinterpret_cast<DWORD>(_RtlDispatchExceptionAddress);
    if (RtlDispatchExceptionAddr)
    {
        fn_RtlDispatchException = reinterpret_cast<t_RtlDispatchException>(RtlDispatchExceptionAddr);
        if (fn_RtlDispatchException(&exr, &ctx))
            return 2;
    }
    return 0;
}
LeagueDecryptData LeagueDecrypt::Decrypt(const wchar_t* szModule) {
    std::string szSectionName = ".text";
    LeagueDecryptData ldd;
    ldd.totalFailedDecrypted = 0;
    ldd.totalSuccessDecrypted = 0;
    ldd.totalSuccess_PAGE_NOACCESS = 0;
    ldd.totalSuccess_EXCEPTION_CONTINUE_EXECUTION = 0;
    DWORD dllImageBase = (DWORD)GetModuleHandle(szModule); //suppose hModule is the handle to the loaded Module (.exe or .dll)
    if (dllImageBase) {
        //get the address of NT Header
        IMAGE_NT_HEADERS* pNtHdr = ImageNtHeader((PVOID)dllImageBase);

        //after Nt headers comes the table of section, so get the addess of section table
        IMAGE_SECTION_HEADER* pSectionHdr = (IMAGE_SECTION_HEADER*)(pNtHdr + 1);

        ImageSectionInfo* pSectionInfo = NULL;

        //iterate through the list of all sections, and check the section name in the if conditon. etc
        for (int i = 0; i < pNtHdr->FileHeader.NumberOfSections; i++)
        {
            char* name = (char*)pSectionHdr->Name;
            if (memcmp(name, szSectionName.c_str(), szSectionName.length()) == 0)
            {
                pSectionInfo = new ImageSectionInfo(szSectionName.c_str());
                pSectionInfo->SectionAddress = dllImageBase + pSectionHdr->VirtualAddress;
                pSectionInfo->SectionSize = pSectionHdr->Misc.VirtualSize;
                auto mbi = MEMORY_BASIC_INFORMATION{ nullptr };
                uint8_t* nextCheckAddress = NULL;
                bool isFirstRegion = true;
                for (unsigned int i = 0; i < pSectionInfo->SectionSize; ++i)
                {
                    uint8_t* currentAddress = (uint8_t*)pSectionInfo->SectionAddress + i;
                    if (currentAddress >= nextCheckAddress)
                    {
                        if (!VirtualQuery(reinterpret_cast<void*>(currentAddress), &mbi, sizeof(mbi)))
                            continue;

                        if (mbi.Protect != PAGE_NOACCESS || isFirstRegion)
                        {
                            isFirstRegion = false;
                            i += reinterpret_cast<std::uintptr_t>(mbi.BaseAddress) + mbi.RegionSize - (pSectionInfo->SectionAddress + i);
                            i--;
                            continue;
                        }
                        nextCheckAddress = static_cast<uint8_t*>(mbi.BaseAddress) + mbi.RegionSize;
                    }
                    const int ret = IsMemoryDecrypted(reinterpret_cast<PVOID>(reinterpret_cast<DWORD>(currentAddress)));
                    if (ret != 0)
                    {
                        if (ret == 1)
                            ldd.totalSuccess_PAGE_NOACCESS++;

                        else if (ret == 2)
                            ldd.totalSuccess_EXCEPTION_CONTINUE_EXECUTION++;

                        ldd.totalSuccessDecrypted++;
                    }
                    else
                    {
                        ldd.totalFailedDecrypted++;
                    }
                }
                break;
            }
            pSectionHdr++;
        }
    }
    return ldd;
}
