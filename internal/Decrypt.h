#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <cstdint>
#include <string>
#include <process.h>
#include <vector>
#include <ImageHlp.h>

struct LeagueDecryptData
{
    int totalSuccessDecrypted = 0;
    int totalSuccess_PAGE_NOACCESS = 0;
    int totalSuccess_EXCEPTION_CONTINUE_EXECUTION = 0;
    int totalFailedDecrypted = 0;
    LeagueDecryptData& operator+=(const LeagueDecryptData& ldd) {
        totalSuccessDecrypted += ldd.totalSuccessDecrypted;
        totalSuccess_PAGE_NOACCESS += ldd.totalSuccess_PAGE_NOACCESS;
        totalSuccess_EXCEPTION_CONTINUE_EXECUTION += ldd.totalSuccess_EXCEPTION_CONTINUE_EXECUTION;
        totalFailedDecrypted += ldd.totalFailedDecrypted;
        return *this; // return the result by reference
    }
};
struct ImageSectionInfo
{
    char SectionName[IMAGE_SIZEOF_SHORT_NAME];//the macro is defined WinNT.h
    DWORD SectionAddress;
    DWORD SectionSize;
    ImageSectionInfo(const char* name)
    {
        strcpy_s(SectionName, name);
    }
};

typedef BOOLEAN(__stdcall* t_RtlDispatchException)(PEXCEPTION_RECORD exceptionRecord, PCONTEXT contextRecord);

class LeagueDecrypt {
public:
	uint8_t* FindRtlDispatchExceptionAddress();
	int IsMemoryDecrypted(PVOID address);
	LeagueDecryptData Decrypt(const wchar_t* szModule);
    uint8_t* _RtlDispatchExceptionAddress;
	uint8_t* FindSignature(const LPCSTR szModule, const char* szSignature);
private:
    t_RtlDispatchException fn_RtlDispatchException;
};