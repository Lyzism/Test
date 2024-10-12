// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "../kiero/kiero.h"

// Typedef untuk fungsi Present
typedef HRESULT(__stdcall* Present)(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags);
Present oPresent = NULL;

// Fungsi untuk mengalokasikan console
void CreateConsole()
{
    // Alokasikan console baru
    AllocConsole();

    // Hubungkan std::cout ke console baru
    FILE* fileStream;
    freopen_s(&fileStream, "CONOUT$", "w", stdout);

    // Sekarang kita bisa menggunakan std::cout untuk menulis ke console
    std::cout << "Console berhasil dibuat!" << std::endl;
}

// Hook function untuk Present
HRESULT __stdcall hkPresent(IDXGISwapChain* pSwapChain, UINT SyncInterval, UINT Flags)
{
    std::cout << "Hook Present berhasil dipanggil!" << std::endl;
    // Panggil fungsi Present asli
    return oPresent(pSwapChain, SyncInterval, Flags);
}

void InitializeKiero()
{
    auto status = kiero::init(kiero::RenderType::D3D11);

    if (status == kiero::Status::Success)
    {
        std::cout << "Kiero successfully initialized!" << std::endl;

        // Hook the Present function (DX11 VTable index 8)
        if (kiero::bind(8, (void**)&oPresent, hkPresent) == kiero::Status::Success)
        {
            std::cout << "Successfully hooked the Present function!" << std::endl;
        }
        else
        {
            std::cout << "Failed to hook the Present function!" << std::endl;
        }
    }
    else
    {
        std::cout << "Kiero initialization failed with status code: " << static_cast<int>(status) << std::endl;
        // Check specific failure cases
        if (status == kiero::Status::ModuleNotFoundError)
        {
            std::cout << "Kiero could not find the appropriate module (e.g., D3D11 DLL)!" << std::endl;
        }
        else if (status == kiero::Status::NotSupportedError)
        {
            std::cout << "Kiero doesn't support this graphics API!" << std::endl;
        }
    }
}

DWORD APIENTRY main_thread(LPVOID)
{
    try
    {
        CreateConsole();
        InitializeKiero();

        while (true)
        {
            std::this_thread::sleep_for(1s);
        }
    }
    catch (const std::exception& ex)
    {
        std::cout << "Error occured: " << ex.what();
    }

    return 0;
}


BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, main_thread, NULL, 0, NULL);
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

