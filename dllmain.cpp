// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include <Windows.h>
#include <iostream>
#include <vector>
#include <TlHelp32.h>
#include <Psapi.h>

uintptr_t GetModuleBaseAddress(DWORD procId, const wchar_t* modName)
{
    uintptr_t modBaseAddr = 0;
    HANDLE hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPMODULE | TH32CS_SNAPMODULE32, procId);
    if (hSnap != INVALID_HANDLE_VALUE)
    {
        MODULEENTRY32 modEntry;
        modEntry.dwSize = sizeof(modEntry);
        if (Module32First(hSnap, &modEntry))
        {
            do
            {
                if (!_wcsicmp(modEntry.szModule, modName))
                {
                    modBaseAddr = (uintptr_t)modEntry.modBaseAddr;
                    break;
                }
            } while (Module32Next(hSnap, &modEntry));
        }
    }
    CloseHandle(hSnap);
    return modBaseAddr;
}

uintptr_t FindDMAAddy(HANDLE hProc, uintptr_t ptr, std::vector<unsigned int> offsets)
{
    uintptr_t addr = ptr;
    for (unsigned int i = 0; i < offsets.size(); ++i)
    {
        ReadProcessMemory(hProc, (BYTE*)addr, &addr, sizeof(addr), 0);
        addr += offsets[i];
    }
    return addr;
}

DWORD WINAPI Thread(HMODULE hModule)
{
    HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, NULL, GetCurrentProcessId());
    auto baseaddress = GetModuleBaseAddress(GetCurrentProcessId(), L"HelloNeighborReborn-Win64-Shipping.exe");
    auto address = baseaddress + 0x02388298;
    std::vector<unsigned int> a = { 0x250,0x0,0x1F8,0x350,0x260,0x364 };
    float value;
    float defaultvalue = 112;
    while (true) {
        ReadProcessMemory(process, (float*)FindDMAAddy(process, address, a), &value, 4, NULL);
        if (value == 90) {
            WriteProcessMemory(process, (float*)FindDMAAddy(process, address, a), &defaultvalue, 4, NULL);
        }
        std::cout << value << std::endl;
    }
    return 0;
}



BOOL APIENTRY DllMain(HMODULE hModule,
    DWORD  ul_reason_for_call,
    LPVOID lpReserved
)
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CloseHandle(CreateThread(nullptr, 0, (LPTHREAD_START_ROUTINE)Thread, hModule, 0, nullptr));
    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

