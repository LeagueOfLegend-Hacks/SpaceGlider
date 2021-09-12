#include <string>
#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#define _SILENCE_EXPERIMENTAL_FILESYSTEM_DEPRECATION_WARNING
#include <experimental/filesystem>
namespace fs = std::experimental::filesystem;

using namespace std;

bool GetProcessEntryByName(string name, PROCESSENTRY32* pe) {
    auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        cerr << "Tool helper cannot be created" << endl;
        return false;
    }

    if (!Process32First(snapshot, pe)) {
        cerr << "Tool helper cannot retrieve the first entry of process list" << endl;
        return false;
    }

    do {
        wstring ws(pe->szExeFile);
        std::string str(ws.begin(), ws.end());
        if (str == name) {
            snapshot ? CloseHandle(snapshot) : 0;
            return true;
        }
    } while (Process32Next(snapshot, pe));

    snapshot ? CloseHandle(snapshot) : 0;
    return false;
}

int main(int argc, const char* argv[]) {
    PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
    string fullPath;

    char buf[MAX_PATH] = { 0 };
    auto dllpath = fs::current_path().string() + "/" + "SpaceGlider.exe";
    GetFullPathNameA(dllpath.c_str(), MAX_PATH, buf, nullptr);
    fullPath = string(buf, MAX_PATH);

    for (; !GetProcessEntryByName("League of Legends.exe", &pe); Sleep(100));

    Sleep(5000);

    auto process = OpenProcess(PROCESS_CREATE_THREAD | PROCESS_QUERY_INFORMATION | PROCESS_VM_READ | PROCESS_VM_WRITE | PROCESS_VM_OPERATION, false, pe.th32ProcessID);
    if (!process) {
        cerr << "Process cannot be opened" << endl;
        exit(0);
    }

    auto fpLoadLibrary = (LPVOID)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryA");

    auto mem = VirtualAllocEx(process, NULL, fullPath.length() + 1, MEM_RESERVE | MEM_COMMIT, PAGE_READWRITE);
    if (!mem) {
        cerr << "Library name cannot be allocated" << endl;
        exit(0);
    }

    if (!WriteProcessMemory(process, mem, fullPath.c_str(), fullPath.length() + 1, nullptr)) {
        cerr << "Library name cannot be written" << endl;
        exit(0);
    }

    if (!CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)fpLoadLibrary, mem, 0, nullptr)) {
        cerr << "Threads cannot be created" << endl;
        exit(0);
    }
    else {
        cout << "Injected" << endl;
    }

    process ? CloseHandle(process) : 0;
    return 0;
exit:
    system("PAUSE");
    return 0;
}