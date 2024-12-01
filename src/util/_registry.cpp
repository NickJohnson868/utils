#ifdef _WIN32

#include "_registry.h"
#include "util.h"
#include "_string.h"

#include <iostream>

#define str(s) CStringUtil::wstring_to_multibyte(s, CP_ACP)

bool CRegistryUtil::read_registry_key(HKEY hKeyParent, const std::wstring& subkey, const std::wstring& valueName, DWORD dwType, std::wstring& data)
{
	HKEY hKey = nullptr;
	try
	{
		if (RegOpenKeyExW(hKeyParent, subkey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		{
			Util::color_print(P_ERROR, "Failed to open registry key: %s\n", str(subkey).c_str());
			return false;
		}

		DWORD dwSize = 0;
		if (RegQueryValueExW(hKey, valueName.c_str(), NULL, NULL, NULL, &dwSize) != ERROR_SUCCESS)
		{
			Util::color_print(P_ERROR, "Failed to query size for value: %s\n", str(valueName).c_str());
			RegCloseKey(hKey);
			return false;
		}

		if (dwType == REG_SZ || dwType == REG_EXPAND_SZ)
		{
			std::vector<wchar_t> value(dwSize / sizeof(wchar_t) + 1); // +1 for null-terminator
			if (RegQueryValueExW(hKey, valueName.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&value[0]), &dwSize) == ERROR_SUCCESS)
			{
				data.assign(value.begin(), value.end() - 1); // Exclude the null-terminator
			}
			else
			{
				Util::color_print(P_ERROR, "Failed to read value: %s\n", str(valueName).c_str());
				RegCloseKey(hKey);
				return false;
			}
		}
		else
		{
			Util::color_print(P_ERROR, "Unsupported registry type: %u\n", dwType);
			RegCloseKey(hKey);
			return false;
		}

		RegCloseKey(hKey);
		return true;
	}
	catch (const std::exception& e)
	{
		if (hKey) RegCloseKey(hKey);
		Util::color_print(P_ERROR, "Exception in read_registry_key: %s\n", e.what());
		return false;
	}
}

bool CRegistryUtil::write_registry_key(HKEY hKeyParent, const std::wstring& subkey, const std::wstring& valueName, DWORD dwType, const std::wstring& data)
{
	HKEY hKey = nullptr;
	try
	{
		if (RegCreateKeyExW(hKeyParent, subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
		{
			Util::color_print(P_ERROR, "Failed to create or open registry key: %s\n", str(subkey).c_str());
			return false;
		}

		if (dwType == REG_SZ || dwType == REG_EXPAND_SZ)
		{
			if (RegSetValueExW(hKey, valueName.c_str(), 0, dwType, (const BYTE*)data.c_str(), (DWORD)(data.size() + 1) * sizeof(wchar_t)) != ERROR_SUCCESS)
			{
				Util::color_print(P_ERROR, "Failed to set value: %s\n", str(valueName).c_str());
				RegCloseKey(hKey);
				return false;
			}
		}
		else
		{
			Util::color_print(P_ERROR, "Unsupported registry type: %u\n", dwType);
			RegCloseKey(hKey);
			return false;
		}

		RegCloseKey(hKey);
		return true;
	}
	catch (const std::exception& e)
	{
		if (hKey) RegCloseKey(hKey);
		Util::color_print(P_ERROR, "Exception in write_registry_key: %s\n", e.what());
		return false;
	}
}

bool CRegistryUtil::delete_registry_key(HKEY hKeyParent, const std::wstring& subkey)
{
	LONG lResult = RegDeleteKeyW(hKeyParent, subkey.c_str());
	if (lResult != ERROR_SUCCESS)
	{
		Util::color_print(P_ERROR, "Failed to delete registry key: %s, error code: %d\n", str(subkey).c_str(), lResult);
	}
	return lResult == ERROR_SUCCESS;
}

#endif