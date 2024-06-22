#define UNICODE
#pragma comment(lib, "user32")
#pragma comment(lib, "kernel32")
#include <assert.h>
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <string>

uint16_t read_last_two_bytes(HANDLE hfile) {
    DWORD file_size = GetFileSize(hfile, nullptr);
    SetFilePointer(hfile, file_size - 2, 0, FILE_BEGIN);
    unsigned char buf[2];
    ReadFile(hfile, buf, 2, nullptr, nullptr);
    uint16_t lo = buf[0];
    uint16_t hi = buf[1];
    return (hi << 8) + lo;
}

void write_last_two_bytes(HANDLE hfile, uint16_t two_bytes) {
    DWORD file_size = GetFileSize(hfile, nullptr);
    SetFilePointer(hfile, file_size - 2, 0, FILE_BEGIN);
    unsigned char buf[2];
    buf[0] = two_bytes & 255;
    buf[1] = two_bytes >> 8;
    WriteFile(hfile, buf, 2, nullptr, nullptr);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    WCHAR path_c[MAX_PATH];
    GetModuleFileNameW(nullptr, path_c, MAX_PATH);
    std::wstring path(path_c);
    std::wstring tmp = path;
    tmp = tmp.substr(0, tmp.size() - 4) + L"_tmp.exe";

    if (wcslen(pCmdLine) == 0) {
        if (!CopyFileW(path_c, tmp.c_str(), false)) {
            MessageBoxW(nullptr, L"Failed to copy to tmp file", L"Self Updater Error", MB_OK | MB_ICONSTOP);
            return 0;
        }

        tmp = L"\"" + tmp + L"\" /U";
        wchar_t tmp_buf[MAX_PATH];
        int i;
        for (i = 0; i < MAX_PATH - 1 && i < tmp.size(); i++) {
            tmp_buf[i] = tmp[i];
        }
        tmp_buf[i] = '\0';

        STARTUPINFO startup_info{};
        PROCESS_INFORMATION proc_info{};
        startup_info.cb = sizeof(startup_info);
        CreateProcessW(nullptr, tmp_buf, nullptr, nullptr, false, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr, &startup_info, &proc_info);
        TerminateProcess(GetCurrentProcess(), 0);
        ExitProcess(0);
    }

    if (wcscmp(pCmdLine, L"/U") == 0) {
        std::wstring orig = path.substr(0, path.size() - 8) + L".exe";
        HANDLE hfile = CreateFileW(orig.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hfile == INVALID_HANDLE_VALUE) {
            wchar_t buf[120];
            wsprintf(buf, L"Failed to create file handle. (/U) GetLastError: %u", GetLastError());
            MessageBoxW(nullptr, buf, L"Self Updater Error", MB_OK | MB_ICONSTOP);
            return 0;
        }
        // wchar_t buf[120];
        // wsprintf(buf, L"Test: %hu", read_last_two_bytes(hfile));
        // MessageBoxW(nullptr, buf, L"Test", MB_OK);
        write_last_two_bytes(hfile, read_last_two_bytes(hfile) + 1);
        CloseHandle(hfile);

        path = L"\"" + orig + L"\" /M";
        wchar_t path_buf[MAX_PATH];
        int i;
        for (i = 0; i < MAX_PATH - 1 && i < path.size(); i++) {
            path_buf[i] = path[i];
        }
        path_buf[i] = '\0';

        STARTUPINFO startup_info{};
        PROCESS_INFORMATION proc_info{};
        startup_info.cb = sizeof(startup_info);
        CreateProcessW(nullptr, path_buf, nullptr, nullptr, false, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr, &startup_info, &proc_info);
        TerminateProcess(GetCurrentProcess(), 0);
        ExitProcess(0);
    }

    if (wcscmp(pCmdLine, L"/M") == 0) {
        DeleteFileW(tmp.c_str());
    
        HANDLE hfile = CreateFileW(path_c, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hfile == INVALID_HANDLE_VALUE) {
            MessageBoxW(nullptr, L"Failed to create file handle. (/M)", L"Self Updater Error", MB_OK | MB_ICONSTOP);
            return 0;
        }
        wchar_t buf[50];
        wsprintf(buf, L"Number: %hu", read_last_two_bytes(hfile));
        MessageBoxW(nullptr, buf, L"Number", MB_OK);
        return 0;
    }

    if (wcscmp(pCmdLine, L"/R") == 0 || wcscmp(pCmdLine, L"/r") == 0) {
        if (!CopyFileW(path_c, tmp.c_str(), false)) {
            MessageBoxW(nullptr, L"Failed to copy to tmp file (/R)", L"Self Updater Error", MB_OK | MB_ICONSTOP);
            return 0;
        }

        tmp = L"\"" + tmp + L"\" /R2";
        wchar_t tmp_buf[MAX_PATH];
        int i;
        for (i = 0; i < MAX_PATH - 1 && i < tmp.size(); i++) {
            tmp_buf[i] = tmp[i];
        }
        tmp_buf[i] = '\0';

        STARTUPINFO startup_info{};
        PROCESS_INFORMATION proc_info{};
        startup_info.cb = sizeof(startup_info);
        CreateProcessW(nullptr, tmp_buf, nullptr, nullptr, false, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr, &startup_info, &proc_info);
        TerminateProcess(GetCurrentProcess(), 0);
        ExitProcess(0);
    }

    if (wcscmp(pCmdLine, L"/R2") == 0) {
        std::wstring orig = path.substr(0, path.size() - 8) + L".exe";
        HANDLE hfile = CreateFileW(orig.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hfile == INVALID_HANDLE_VALUE) {
            wchar_t buf[120];
            wsprintf(buf, L"Failed to create file handle. (/R2) GetLastError: %u", GetLastError());
            MessageBoxW(nullptr, buf, L"Self Updater Error", MB_OK | MB_ICONSTOP);
            return 0;
        }
        write_last_two_bytes(hfile, 65535);
        CloseHandle(hfile);

        path = L"\"" + orig + L"\" /RT";
        wchar_t path_buf[MAX_PATH];
        int i;
        for (i = 0; i < MAX_PATH - 1 && i < path.size(); i++) {
            path_buf[i] = path[i];
        }
        path_buf[i] = '\0';

        STARTUPINFO startup_info{};
        PROCESS_INFORMATION proc_info{};
        startup_info.cb = sizeof(startup_info);
        CreateProcessW(nullptr, path_buf, nullptr, nullptr, false, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr, &startup_info, &proc_info);
        TerminateProcess(GetCurrentProcess(), 0);
        ExitProcess(0);
    }

    if (wcscmp(pCmdLine, L"/RT") == 0) {
        DeleteFileW(tmp.c_str());
        return 0;
    }

    return 0;
}
