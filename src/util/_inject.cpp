#ifdef _WIN32

#include <iostream>
#include <windows.h>
#include <tchar.h>
#include <Tlhelp32.h>
#include <string>
#include <set>

#include "_inject.h"

#include "_string.h"

/* 提升进程访问权限 */
bool CInjectUtil::enable_debug_priv()
{
    HANDLE hToken;
    LUID sedebugnameValue;
    TOKEN_PRIVILEGES tkp;
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_ADJUST_PRIVILEGES | TOKEN_QUERY, &hToken))
    {
        return false;
    }
    if (!LookupPrivilegeValue(NULL, SE_DEBUG_NAME, &sedebugnameValue))
    {
        CloseHandle(hToken);
        return false;
    }
    tkp.PrivilegeCount = 1;
    tkp.Privileges[0].Luid = sedebugnameValue;
    tkp.Privileges[0].Attributes = SE_PRIVILEGE_ENABLED;
    if (!AdjustTokenPrivileges(hToken, FALSE, &tkp, sizeof(tkp), NULL, NULL))

    {
        CloseHandle(hToken);
        return false;
    }
    return true;
}


/* 卸载DLL */
bool CInjectUtil::uninject_dll(DWORD dwProcessId, std::wstring dll_name)
{
    if (dwProcessId == 0)
    {
        MessageBoxA(NULL,
                    "The target process have not been found !",
                    "Notice",
                    MB_ICONINFORMATION | MB_OK
        );
        return false;
    }

    if (dll_name == L"")
        return false;

    HANDLE hModuleSnap = INVALID_HANDLE_VALUE;
    HANDLE hProcess = NULL;
    HANDLE hThread = NULL;
    /* 获取模块快照 */
    hModuleSnap = ::CreateToolhelp32Snapshot(TH32CS_SNAPMODULE, dwProcessId);
    if (INVALID_HANDLE_VALUE == hModuleSnap)

    {
        return false;
    }
    MODULEENTRY32 me32;
    memset(&me32, 0, sizeof(MODULEENTRY32));
    me32.dwSize = sizeof(MODULEENTRY32);
    /* 开始遍历 */
    if (FALSE == ::Module32First(hModuleSnap, &me32))
    {
        ::CloseHandle(hModuleSnap);
        return false;
    }
    /* 遍历查找指定模块 */
    bool isFound = false;
    do
    {
#ifdef __MINGW32__
        std::string dll_name_a = CStringUtil::wstring_to_multibyte(dll_name, CP_GBK);
        isFound = (0 == ::lstrcmpiA(me32.szModule, dll_name_a.c_str()) || 0 == ::lstrcmpiA(
            me32.szExePath, dll_name_a.c_str()));
#else
		isFound = (0 == ::lstrcmpiW(me32.szModule, dll_name.c_str()) || 0 == ::lstrcmpiW(me32.szExePath, dll_name.c_str()));
#endif
        if (isFound) /* 找到指定模块 */

        {
            break;
        }
    }
    while (TRUE == ::Module32Next(hModuleSnap, &me32));
    ::CloseHandle(hModuleSnap);
    if (false == isFound)

    {
        std::cout << "找不到DLL" << std::endl;
        return false;
    }
    /* 获取目标进程句柄 */
    hProcess = ::OpenProcess(PROCESS_ALL_ACCESS, FALSE, dwProcessId);
    if (NULL == hProcess)
    {
        return false;
    }
    /* 从 Kernel32.dll 中获取 FreeLibrary 函数地址 */
    LPTHREAD_START_ROUTINE lpThreadFun = (PTHREAD_START_ROUTINE)::GetProcAddress(
        ::GetModuleHandle(_T("Kernel32")), "FreeLibrary");
    if (NULL == lpThreadFun)
    {
        ::CloseHandle(hProcess);
        return false;
    }
    /* 创建远程线程调用 FreeLibrary */
    hThread = ::CreateRemoteThread(hProcess, NULL, 0, lpThreadFun, me32.modBaseAddr /* 模块地址 */, 0, NULL);
    if (NULL == hThread)
    {
        ::CloseHandle(hProcess);
        return false;
    }
    /* 等待远程线程结束 */
    ::WaitForSingleObject(hThread, INFINITE);
    /* 清理 */
    ::CloseHandle(hThread);
    ::CloseHandle(hProcess);
    std::cout << "卸载成功" << std::endl;
    return true;
}


/* 注入DLL */
bool CInjectUtil::inject_dll(DWORD pid, std::wstring dll_path)
{
    HANDLE hprocess = OpenProcess(PROCESS_ALL_ACCESS, false, pid);
    if (!hprocess)
    {
        std::cout << "无法获取进程句柄" << std::endl;
        return false;
    }

    SIZE_T PathSize = (dll_path.length() + 1) * sizeof(wchar_t);
    LPVOID StartAddress = VirtualAllocEx(hprocess, NULL, PathSize, MEM_COMMIT, PAGE_READWRITE);
    if (!StartAddress)

    {
        std::cout << "开辟内存失败" << std::endl;
        return false;
    }
    if (!WriteProcessMemory(hprocess, StartAddress, dll_path.c_str(), PathSize, NULL))

    {
        std::cout << "无法写入DLL路径" << std::endl;
        return false;
    }

#ifdef __MINGW32__
    PTHREAD_START_ROUTINE pfnStartAddress = (PTHREAD_START_ROUTINE)GetProcAddress(
        GetModuleHandle("kernel32.dll"), "LoadLibraryW");
#else
	PTHREAD_START_ROUTINE pfnStartAddress = (PTHREAD_START_ROUTINE)GetProcAddress(GetModuleHandle(L"kernel32.dll"), "LoadLibraryW");
#endif
    if (!pfnStartAddress)

    {
        std::cout << "无法获取函数地址" << std::endl;
        return false;
    }
    HANDLE hThread = CreateRemoteThreadEx(hprocess, NULL, 0, pfnStartAddress, StartAddress, 0, NULL, NULL);
    if (!hThread)
    {
        std::cout << "创建线程失败" << std::endl;
        return false;
    }
    std::cout << "注入成功" << std::endl;

    /* 等待DLL结束 */
    WaitForSingleObject(hThread, INFINITE);
    CloseHandle(hThread);
    CloseHandle(hprocess);
    return true;
}

#endif