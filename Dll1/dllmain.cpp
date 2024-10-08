// dllmain.cpp : Defines the entry point for the DLL application.
#include "pch.h"
#include "../kiero/kiero.h"

#define KIERO_INCLUDE_D3D11 1
#define KIERO_USE_MINHOOK 1

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

DWORD WINAPI InitializeHook(LPVOID lpReserved)
{
    // Buat console baru untuk menampilkan log
    CreateConsole();

    // Initialize Kiero
    if (kiero::init(kiero::RenderType::D3D12) == kiero::Status::Success)
    {
        std::cout << "Kiero berhasil diinisialisasi DX 12!" << kiero::getMethodsTable()[8] << std::endl;

        // Hook fungsi Present (IDXGISwapChain::Present adalah index ke-8)
        //kiero::bind(8, (void**)&oPresent, hkPresent);
    }
    else if (kiero::init(kiero::RenderType::D3D11) == kiero::Status::Success)
    {
        std::cout << "Kiero berhasil diinisialisasi DX11!" << kiero::getMethodsTable()[8] << std::endl;

        // Hook fungsi Present (IDXGISwapChain::Present adalah index ke-8)
        kiero::bind(8, (void**)&oPresent, hkPresent);
    }
    else if (kiero::init(kiero::RenderType::D3D9) == kiero::Status::Success)
    {
        std::cout << "Kiero berhasil diinisialisasi DX9!" << kiero::getMethodsTable()[8] << std::endl;

        // Hook fungsi Present (IDXGISwapChain::Present adalah index ke-8)
        //kiero::bind(8, (void**)&oPresent, hkPresent);
    }
    else
    {
        std::cout << "Kiero gagal diinisialisasi dengan status: " << std::endl;
    }

    return TRUE;
}

BOOL APIENTRY DllMain( HMODULE hModule,
                       DWORD  ul_reason_for_call,
                       LPVOID lpReserved
                     )
{
    switch (ul_reason_for_call)
    {
    case DLL_PROCESS_ATTACH:
        CreateThread(NULL, 0, InitializeHook, NULL, 0, NULL);
        break;

    case DLL_THREAD_ATTACH:
    case DLL_THREAD_DETACH:
    case DLL_PROCESS_DETACH:
        break;
    }
    return TRUE;
}

