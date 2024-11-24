#include "_proc.h"

#include <TlHelp32.h>

void _Proc::close_process_by_name(LPCWSTR proc_name)
{
	std::set<DWORD> id_list = get_proc_id_by_name(proc_name);
	for (auto& id : id_list)
	{
		HANDLE hProecss = OpenProcess(PROCESS_ALL_ACCESS, FALSE, id); // 打开进程
		if (hProecss)
		{
			TerminateProcess(hProecss, 0); // 关闭进程
			CloseHandle(hProecss);
		}
	}
}

std::set<unsigned long> _Proc::get_proc_id_by_name(LPCWSTR proc_name)
{
	std::set<DWORD> IDList;
	PROCESSENTRY32 pe = { sizeof(PROCESSENTRY32) };
	HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (hSnapshot != INVALID_HANDLE_VALUE)
	{
		if (Process32First(hSnapshot, &pe))
		{
			while (Process32Next(hSnapshot, &pe))
			{
				if (lstrcmpiW(proc_name, pe.szExeFile) == 0)
				{
					IDList.insert(pe.th32ProcessID);
				}
			}
		}
		CloseHandle(hSnapshot);
	}
	return IDList;
}