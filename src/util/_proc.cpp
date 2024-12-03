#include "_proc.h"
#include "util.h"
#include "_string.h"

#include <iostream>

#ifdef WIN
#include <windows.h>
#include <TlHelp32.h>
#elif defined(LINUX)
#include <dirent.h>
#include <sys/types.h>
#include <unistd.h>
#include <cstring>
#endif

void CProcUtil::close_process_by_name(const PROC_TYPE proc_name)
{
	std::set<PID_TYPE> id_list = get_proc_id_by_name(proc_name);
	for (auto& id : id_list)
	{
		close_process_by_id(id);
	}
}

void CProcUtil::close_process_by_id(const PID_TYPE proc_id)
{
#ifdef WIN
	HANDLE hProecss = OpenProcess(PROCESS_ALL_ACCESS, FALSE, proc_id); // 打开进程
	if (hProecss)
	{
		TerminateProcess(hProecss, 0); // 关闭进程
		CloseHandle(hProecss);
	}
#elif defined(LINUX)
	kill(proc_id, SIGTERM);  // 尝试终止进程
#endif
}

std::set<PID_TYPE> CProcUtil::get_proc_id_by_name(const PROC_TYPE proc_name)
{
	std::set<PID_TYPE> id_list;
#ifdef MSVC
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		if (Process32First(hSnapshot, &pe))
		{
			while (Process32Next(hSnapshot, &pe))
			{
#ifdef MINGW
				if (CStringUtil::compare_ignore_case<std::string>(proc_name, pe.szExeFile))
#elif defined(MSVC)
				if (CStringUtil::compare_ignore_case<std::wstring>(proc_name, pe.szExeFile))
#endif
				{
					id_list.insert(pe.th32ProcessID);
				}
			}
		}
		CloseHandle(hSnapshot);
	}
	return id_list;
#elif defined(LINUX)
	DIR* dir = opendir("/proc");
	if (dir == nullptr) {
		Util::color_print(P_ERROR, "Failed to open /proc");
		return id_list;
	}

	struct dirent* entry;
	while ((entry = readdir(dir)) != nullptr) {
		// 确保是一个进程目录（以数字命名）
		if (isdigit(entry->d_name[0])) {
			std::string pid_dir = std::string("/proc/") + entry->d_name + "/comm";
			FILE* file = fopen(pid_dir.c_str(), "r");
			if (file) {
				char buf[256];
				if (fgets(buf, sizeof(buf), file)) {
					// 移除换行符
					buf[strcspn(buf, "\n")] = 0;
					std::string name = CStringUtil::trim(buf);
					if (proc_name == name) {
						id_list.insert(atoi(entry->d_name));  // 获取进程ID
					}
				}
				fclose(file);
			}
		}
	}
	closedir(dir);
	return id_list;
#endif
}