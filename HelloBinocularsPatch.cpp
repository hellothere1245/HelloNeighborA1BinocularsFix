// HelloBinocularsPatch.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <Windows.h>
#include <TlHelp32.h>

int main()
{
    const char* dllPath = "FixBinocularsPatch.dll";
    LPCTSTR ProcessName = L"HelloNeighborReborn-Win64-Shipping.exe";
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    TCHAR cmdArgs[] = TEXT("HelloNeighborReborn-Win64-Shipping.exe -nohmd");



    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    CreateProcess(L"HelloNeighborReborn-Win64-Shipping.exe",   // No module name (use command line)
        cmdArgs,        // Command line
        NULL,           // Process handle not inheritable
        NULL,           // Thread handle not inheritable
        FALSE,          // Set handle inheritance to FALSE
        0,              // No creation flags
        NULL,           // Use parent's environment block
        NULL,           // Use parent's starting directory 
        &si,            // Pointer to STARTUPINFO structure
        &pi);



    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    HANDLE hProc = OpenProcess(PROCESS_ALL_ACCESS, 0, pi.dwProcessId);
    if (hProc && hProc != INVALID_HANDLE_VALUE) 
    {
        void* loc = VirtualAllocEx(hProc, 0, MAX_PATH, MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
        if (loc) {
            WriteProcessMemory(hProc, loc, dllPath, strlen(dllPath) + 1, 0);
        }
        

        HANDLE hThread = CreateRemoteThread(hProc, 0, 0, (LPTHREAD_START_ROUTINE)LoadLibraryA, loc, 0, 0);

        if (hThread) {
            CloseHandle(hThread);
        }
    }
    if (hProc) {
        CloseHandle(hProc);
    }
    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
