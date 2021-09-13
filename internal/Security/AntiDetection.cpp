#include "AntiDetection.h"

Security::tModule32First Security::oModule32First;
Security::tModule32Next Security::oModule32Next;
Security::tReadProcessMemory Security::oReadProcessMemory;
BOOL __stdcall Security::hkReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead)
{
    return 0;
}
bool __stdcall Security::hkModule32Next(HANDLE hSnapshot, LPMODULEENTRY32 lpme)
{
    ZeroMemory(lpme->szModule, MAX_MODULE_NAME32);
    lpme->modBaseAddr = 0;
    lpme->modBaseSize = 0;
    lpme->hModule = NULL;
    lpme->th32ModuleID = 0;
    lpme->th32ProcessID = 0;
    SetLastError(ERROR_NO_MORE_FILES);
    return FALSE;

    BOOL bReturn = oModule32Next(hSnapshot, lpme);

    return bReturn;
}

bool __stdcall Security::hkModule32First(HANDLE hSnapshot, LPMODULEENTRY32 lpme)
{
    return oModule32First(hSnapshot, lpme);
}
