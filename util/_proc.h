#pragma once

#include <set>
#include <windows.h>

class _Proc
{
public:
	static std::set<unsigned long> get_proc_id_by_name(LPCWSTR proc_name);
	static void close_process_by_name(LPCWSTR proc_name);
};