#include "Cheat.h"
#include <iostream>
#include <TlHelp32.h>
#include <windows.h>
#include <vector>

DWORD Cheat::findProcessId(const std::wstring& processName)
{
    PROCESSENTRY32 processEntry;
    processEntry.dwSize = sizeof(PROCESSENTRY32);

    HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (snapshot == INVALID_HANDLE_VALUE) {
        return 0;
    }

    if (Process32First(snapshot, &processEntry)) {
        do {
            if (!_wcsicmp(processEntry.szExeFile, processName.c_str())) {
                CloseHandle(snapshot);
                return processEntry.th32ProcessID;
            }
        } while (Process32Next(snapshot, &processEntry));
    }

    CloseHandle(snapshot);
    return 0;
}

DWORD Cheat::findAddress(DWORD processId, const std::wstring& moduleName, const std::wstring& symbolName)
{
    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    MODULEENTRY32 me32;

    hModuleSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, processId);
    if (hModuleSnap == INVALID_HANDLE_VALUE) {
        return 0;
    }

    me32.dwSize = sizeof(MODULEENTRY32);

    if (!Module32First(hModuleSnap, &me32)) {
        CloseHandle(hModuleSnap);
        return 0;
    }

    do {
        if (!_wcsicmp(me32.szModule, moduleName.c_str())) {
            CloseHandle(hModuleSnap);
            return (DWORD)me32.modBaseAddr;
        }
    } while (Module32Next(hModuleSnap, &me32));

    CloseHandle(hModuleSnap);
    return 0;
}

Cheat::Cheat()
{
    processName = L"Minecraft.Windows.exe";
}

Cheat::Cheat(std::wstring processName)
{
    this->processName = processName;
}

DWORD GetDynamicAddress(HANDLE hProcess, DWORD basePointer, std::vector<DWORD> offsets) {
    DWORD addr = basePointer;
    for (int i = 0; i < offsets.size(); ++i) {
        ReadProcessMemory(hProcess, (BYTE*)addr, &addr, sizeof(addr), NULL);
        addr += offsets[i];
    }
    return addr;
}

void Cheat::ChangeSpeed(float newSpeed) {
    DWORD processID = findProcessId(processName);
    if (processID == 0) {
        std::cerr << "processus not found" << std::endl;
        return;
    }

    HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, processID);
    if (hProcess == NULL) {
        std::cerr << "impossible to open the processus" << std::endl;
        return;
    }

    DWORD basePointer = 0x2054F20C3BC; 
    std::vector<DWORD> offsets = { 0x28, 0x3B8,  0x40, 0x780, 0x2A8, 0xBBC }; 

    DWORD address = GetDynamicAddress(hProcess, basePointer, offsets);

    float currentSpeed;
    ReadProcessMemory(hProcess, (LPVOID)address, &currentSpeed, sizeof(currentSpeed), NULL);
    std::cout << "current speed : " << currentSpeed << std::endl;

    WriteProcessMemory(hProcess, (LPVOID)address, &newSpeed, sizeof(newSpeed), NULL);
    std::cout << "new speed : " << newSpeed << std::endl;

    CloseHandle(hProcess);
}
