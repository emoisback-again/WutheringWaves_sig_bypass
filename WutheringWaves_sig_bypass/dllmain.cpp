// dllmain.cpp : 定义 DLL 应用程序的入口点。
#include "pch.h"
#include <cstdint>
#include <tchar.h>
#include "HookManager.h"


#define DO_APP_FUNC(a, r, n, p)  r (*n) p
DO_APP_FUNC(0x3CD3FC0, __int64, BroadcastPakChunkSignatureCheckFailure, (__int64 a1, __int64 a2));
DO_APP_FUNC(0x3CD4670, bool, FileCheck, (__int64 a1));
#undef DO_APP_FUNC


uint8_t* GameBase = (uint8_t*)GetModuleHandleA(0);

__int64 BroadcastPakChunkSignatureCheckFailure_Hook(__int64 a1, __int64 a2)
{
    return 1;
}
bool FileCheck_Hook(__int64 a1)
{
    return true;
}
bool sigCheck_bypass()
{
    DWORD oldProtect;
    uint8_t* targetAddress = GameBase + 0x3CD5259;
    unsigned char* address = reinterpret_cast<unsigned char*>(targetAddress);
    if (VirtualProtect(address, 2, PAGE_EXECUTE_READWRITE, &oldProtect))
    {
        address[0] = 0x90;
        address[1] = 0x90;

        VirtualProtect(address, 2, oldProtect, &oldProtect);
    }
    if (address[0] == 0x90)
    {
        if (address[1] = 0x90)
        {
            return true;
        }
        return false;
    }
    return false;
}
void init_static_offsets()
{
    uint8_t* baseAddress = (uint8_t*)GetModuleHandleA(0);

    #define DO_APP_FUNC(a, r, n, p) n = (r (*) p)(baseAddress + a)
    DO_APP_FUNC(0x3CD3FC0, __int64, BroadcastPakChunkSignatureCheckFailure, (__int64 a1, __int64 a2));
    DO_APP_FUNC(0x3CD4670, bool, FileCheck, (__int64 a1));
    #undef DO_APP_FUNC

}

void BaseHook()
{
    HookManager::install(BroadcastPakChunkSignatureCheckFailure, BroadcastPakChunkSignatureCheckFailure_Hook);
    HookManager::install(FileCheck, FileCheck_Hook);
}
DWORD WINAPI MyThreadFunction(LPVOID lpParam)
{
    Sleep(2500);
    while (true)
    {
        if (GetModuleHandleA(0) != 0)
        {
            GameBase = (uint8_t*)GetModuleHandleA(0);
            break;
        }
    }
    init_static_offsets();
    BaseHook();
    sigCheck_bypass();
    return 0;
}
HWND StartConsole(LPCWSTR title, bool close) {
    HANDLE g_hOutput = 0;
    HWND hwnd = NULL;
    HMENU hmenu = NULL;
    AllocConsole();
    g_hOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTitle((LPCSTR)title);
    while (NULL == hwnd) hwnd = ::FindWindow(NULL, (LPCTSTR)title);
    hmenu = ::GetSystemMenu(hwnd, FALSE);
    if (!close) { DeleteMenu(hmenu, SC_CLOSE, MF_BYCOMMAND); }
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_MAXIMIZEBOX);
    SetWindowLong(hwnd, GWL_STYLE, GetWindowLong(hwnd, GWL_STYLE) & ~WS_THICKFRAME);
    SetWindowPos(hwnd, NULL, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE | SWP_FRAMECHANGED);
    freopen_s((FILE**)stdout, "CONOUT$", "w", stdout);
    freopen_s((FILE**)stderr, "CONOUT$", "w", stderr);
    return hwnd;
}
BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        //StartConsole((LPCWSTR)"原神怎么你了", true);
        CreateThread(nullptr, 0, MyThreadFunction, nullptr, 0, nullptr);
        break;
    }
    return TRUE;
}

