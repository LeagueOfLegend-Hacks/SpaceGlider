#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include "../Console/Console.h"
namespace Security {
    typedef bool (WINAPI* tModule32Next)(HANDLE, LPMODULEENTRY32);
    typedef bool (WINAPI* tModule32First)(HANDLE, LPMODULEENTRY32);
    typedef BOOL(WINAPI* tReadProcessMemory) (HANDLE, LPCVOID, LPVOID, SIZE_T, SIZE_T*);
    extern tModule32Next oModule32Next;
    extern tModule32First oModule32First;
    extern tReadProcessMemory oReadProcessMemory;
    BOOL WINAPI hkReadProcessMemory(HANDLE hProcess, LPCVOID lpBaseAddress, LPVOID lpBuffer, SIZE_T nSize, SIZE_T* lpNumberOfBytesRead);
    bool WINAPI hkModule32Next(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
    bool WINAPI hkModule32First(HANDLE hSnapshot, LPMODULEENTRY32 lpme);
}