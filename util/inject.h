#pragma once

#include <Windows.h>
#include <set>
#include <string>

bool enable_debug_priv();

bool uninject_dll(DWORD dwProcessId, std::wstring dll_name);

bool inject_dll(DWORD pid, std::wstring dll_path);