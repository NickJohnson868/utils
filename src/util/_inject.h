#pragma once

#ifdef _WIN32

#include <windows.h>
#include <string>

class CInjectUtil
{
public:
	static bool enable_debug_priv();

	static bool uninject_dll(DWORD dwProcessId, std::wstring dll_name);

	static bool inject_dll(DWORD pid, std::wstring dll_path);
};

#endif