#define NOMINMAX
#include <Windows.h>

#include "valve_sdk/sdk.hpp"
#include "helpers/utils.hpp"
#include "helpers/input.hpp"

#include "hooks.hpp"
#include "menu.hpp"
#include "options.hpp"

#include "resolver.hpp"

DWORD WINAPI OnDllAttach(LPVOID base)
{
    // 
    // Wait at most 10s for the main game modules to be loaded.
    // 
    if(Utils::WaitForModules(10000, { L"client.dll", L"engine.dll", L"shaderapidx9.dll" }) == WAIT_TIMEOUT) {
        // One or more modules were not loaded in time
        return FALSE;
    }

    try {
        Interfaces::Initialize();

        NetvarSys::Get().Initialize();
        InputSys::Get().Initialize();
        Menu::Get().Initialize();

        Hooks::Initialize();

		Resolver::Initialize();

        // Register some hotkeys.
        // - Note:  The function that is called when the hotkey is pressed
        //          is called from the WndProc thread, not this thread.
        // 

        // Menu Toggle
        InputSys::Get().RegisterHotkey(VK_INSERT, [base]() {
            Menu::Get().Toggle();
        });

        while(!g_Unload)
            Sleep(1000);

		Resolver::Uninitialize();

        g_CVar->FindVar("crosshair")->SetValue(true);

		FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
    } catch(const std::exception& ex) {
        FreeLibraryAndExitThread(static_cast<HMODULE>(base), 1);
    }
}

BOOL WINAPI OnDllDetach()
{
    Hooks::Shutdown();

    Menu::Get().Shutdown();
    return TRUE;
}

BOOL WINAPI DllMain(
    _In_      HINSTANCE hinstDll,
    _In_      DWORD     fdwReason,
    _In_opt_  LPVOID    lpvReserved
)
{
    switch(fdwReason) {
        case DLL_PROCESS_ATTACH:
            DisableThreadLibraryCalls(hinstDll);
            CreateThread(nullptr, 0, OnDllAttach, hinstDll, 0, nullptr);
            return TRUE;
        case DLL_PROCESS_DETACH:
            if(lpvReserved == nullptr)
                return OnDllDetach();
            return TRUE;
        default:
            return TRUE;
    }
}
