#pragma once

#include <windows.h>
#include <string>

class _Registry
{
public:
	static bool read_registry_key(HKEY hKeyParent, const std::wstring& subkey, const std::wstring& valueName, DWORD dwType, std::wstring& data);
	static bool write_registry_key(HKEY hKeyParent, const std::wstring& subkey, const std::wstring& valueName, DWORD dwType, const std::wstring& data);
	static bool delete_registry_key(HKEY hKeyParent, const std::wstring& subkey);
};