#include "pch.h"
#include "bootstrap.h"

using namespace engine;

int WINAPI WinMain(HINSTANCE, HINSTANCE, LPSTR lpCmdLine, int)
{
    auto hPixDll = LoadLibraryW(L"C:\\Program Files\\Microsoft PIX\\2601.15\\WinPixGpuCapturer.dll");
    if (hPixDll == nullptr)
    {
        // Windowsから「なぜ失敗したか」のエラー番号を取得する
        DWORD errorCode = GetLastError();

        wchar_t msg[256];
        swprintf_s(msg, L"DLLの読み込みに失敗しました。\nエラーコード: %lu\n\n126: パスが見つからない\n193: 32bit/64bitの不一致", errorCode);
        MessageBoxW(nullptr, msg, L"PIX読み込みエラー", MB_ICONERROR);
    }
    return static_cast<int>(bootstrap::Launch(bootstrap::LaunchOptions::Parse(lpCmdLine)));
}