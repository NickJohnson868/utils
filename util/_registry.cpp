#include "_registry.h"

#include <iostream>

bool _Registry::read_registry_key(HKEY hKeyParent, const std::wstring& subkey, const std::wstring& valueName, DWORD dwType, std::wstring& data)
{
	try {
		HKEY hKey;
		if (RegOpenKeyEx(hKeyParent, subkey.c_str(), 0, KEY_READ, &hKey) != ERROR_SUCCESS)
		{
			return false;
		}

		DWORD dwSize;
		if (RegQueryValueEx(hKey, valueName.c_str(), NULL, NULL, NULL, &dwSize) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return false;
		}

		if (dwType == REG_SZ || dwType == REG_EXPAND_SZ)
		{
			std::wstring value(dwSize / sizeof(wchar_t), L'\0');
			if (RegQueryValueEx(hKey, valueName.c_str(), NULL, NULL, reinterpret_cast<LPBYTE>(&value[0]), &dwSize) == ERROR_SUCCESS)
			{
				data = value;
			}
		}

		RegCloseKey(hKey);
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
}

bool _Registry::write_registry_key(HKEY hKeyParent, const std::wstring& subkey, const std::wstring& valueName, DWORD dwType, const std::wstring& data)
{
	try {
		HKEY hKey;
		if (RegCreateKeyEx(hKeyParent, subkey.c_str(), 0, NULL, REG_OPTION_NON_VOLATILE, KEY_WRITE, NULL, &hKey, NULL) != ERROR_SUCCESS)
		{
			return false;
		}

		if (RegSetValueEx(hKey, valueName.c_str(), 0, dwType, (const BYTE*)data.c_str(), (DWORD)(data.size() + 1) * sizeof(wchar_t)) != ERROR_SUCCESS)
		{
			RegCloseKey(hKey);
			return false;
		}

		RegCloseKey(hKey);
		return true;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return false;
	}
}

bool _Registry::delete_registry_key(HKEY hKeyParent, const std::wstring& subkey)
{
	LONG lResult = RegDeleteKey(hKeyParent, subkey.c_str());
	return lResult == ERROR_SUCCESS;
}