#define UNICODE
#pragma comment(lib, "user32")
#pragma comment(lib, "kernel32")
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <string>

void error_quit(LPCWSTR msg) {
    wchar_t buf[120];
    wsprintf(buf, L"%s\nGetLastError: %u", msg, GetLastError());
    MessageBoxW(nullptr, buf, L"Self Updater Error", MB_OK | MB_ICONSTOP);
    ExitProcess(1);
}

uint16_t read_last_two_bytes(HANDLE hfile) {
    DWORD file_size = GetFileSize(hfile, nullptr);
    SetFilePointer(hfile, file_size - 2, 0, FILE_BEGIN);
    char buf[2];
    ReadFile(hfile, buf, 2, nullptr, nullptr);
    return *(uint16_t *)buf;
}

void write_last_two_bytes(HANDLE hfile, uint16_t two_bytes) {
    DWORD file_size = GetFileSize(hfile, nullptr);
    SetFilePointer(hfile, file_size - 2, 0, FILE_BEGIN);
    WriteFile(hfile, (void *)&two_bytes, 2, nullptr, nullptr);
}

void launch_proc(std::wstring exe, const std::wstring& arg) {
    exe = L"\"" + exe + L"\" " + arg;
    wchar_t cmd_line[MAX_PATH];
    int i;
    for (i = 0; i < MAX_PATH - 1 && i < exe.size(); i++) {
        cmd_line[i] = exe[i];
    }
    cmd_line[i] = '\0';

    STARTUPINFO startup_info{};
    PROCESS_INFORMATION proc_info{};
    startup_info.cb = sizeof(startup_info);
    CreateProcessW(nullptr, cmd_line, nullptr, nullptr, false, CREATE_DEFAULT_ERROR_MODE, nullptr, nullptr, &startup_info, &proc_info);
}

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow) {
    HANDLE hm = CreateMutexW(nullptr, true, L"self updater ultra unique mutex bruh");
    if (GetLastError() == ERROR_ALREADY_EXISTS) {
        MessageBoxW(nullptr, L"Already running.", L"Self Updater Error", MB_OK | MB_ICONSTOP);
        ExitProcess(1);
    }
    ReleaseMutex(hm);

    wchar_t path_c[MAX_PATH];
    GetModuleFileNameW(nullptr, path_c, MAX_PATH);
    std::wstring path(path_c);
    std::wstring tmp = path;
    tmp = tmp.substr(0, tmp.size() - 4) + L"_tmp.exe";
    // start
    if (wcslen(pCmdLine) == 0) {
        if (!CopyFileW(path_c, tmp.c_str(), false)) error_quit(L"Failed to copy to tmp file.");
        launch_proc(tmp, L"/U");
        ExitProcess(0);
    }
    // updater (tmp)
    if (wcscmp(pCmdLine, L"/U") == 0) {
        std::wstring orig = path.substr(0, path.size() - 8) + L".exe";
        HANDLE hfile = CreateFileW(orig.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hfile == INVALID_HANDLE_VALUE) error_quit(L"Failed to create read-write file handle.");
        write_last_two_bytes(hfile, read_last_two_bytes(hfile) + 1);
        CloseHandle(hfile);
        launch_proc(orig, L"/M");
        ExitProcess(0);
    }
    // display the number
    if (wcscmp(pCmdLine, L"/M") == 0) {
        HANDLE hm = CreateMutexW(nullptr, true, L"self updater ultra unique mutex bruh");
        DeleteFileW(tmp.c_str());
        HANDLE hfile = CreateFileW(path_c, GENERIC_READ, FILE_SHARE_READ, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hfile == INVALID_HANDLE_VALUE) error_quit(L"Failed to create read file handle.");
        wchar_t buf[50];
        wsprintf(buf, L"Number: %hu", read_last_two_bytes(hfile));
        MessageBoxW(nullptr, buf, L"Number", MB_OK);
        ReleaseMutex(hm);
        ExitProcess(0);
    }
    // reset
    if (wcscmp(pCmdLine, L"/R") == 0 || wcscmp(pCmdLine, L"/r") == 0) {
        if (!CopyFileW(path_c, tmp.c_str(), false)) error_quit(L"Failed to copy to tmp file.");
        launch_proc(tmp, L"/R2");
        ExitProcess(0);
    }
    // reset stage 2 (tmp)
    if (wcscmp(pCmdLine, L"/R2") == 0) {
        std::wstring orig = path.substr(0, path.size() - 8) + L".exe";
        HANDLE hfile = CreateFileW(orig.c_str(), GENERIC_READ | GENERIC_WRITE, FILE_SHARE_WRITE, nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
        if (hfile == INVALID_HANDLE_VALUE) error_quit(L"Failed to create read-write file handle.");
        write_last_two_bytes(hfile, -1);
        CloseHandle(hfile);
        launch_proc(orig, L"/RT");
        ExitProcess(0);
    }
    // reset stage 3
    if (wcscmp(pCmdLine, L"/RT") == 0) {
        DeleteFileW(tmp.c_str());
        return 0;
    }

    return 0;
}
