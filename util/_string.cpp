#pragma once

#include "_string.h"

#include <windows.h>


std::string _String::utf8_to_gbk(const std::string& str_utf8)
{
	int len = MultiByteToWideChar(CP_UTF8, 0, str_utf8.c_str(), -1, NULL, 0);
	wchar_t* wszGBK = new wchar_t[len + 1];
	memset(wszGBK, 0, len * 2 + 2);
	MultiByteToWideChar(CP_UTF8, 0, str_utf8.c_str(), -1, wszGBK, len);
	len = WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, NULL, 0, NULL, NULL);
	char* szGBK = new char[len + 1];
	memset(szGBK, 0, len + 1);
	WideCharToMultiByte(CP_ACP, 0, wszGBK, -1, szGBK, len, NULL, NULL);
	std::string strTemp(szGBK);
	if (szGBK) delete[] szGBK;
	if (wszGBK) delete[] wszGBK;
	return strTemp;
}

std::string _String::gbk_to_utf8(const std::string& str_gbk)
{
	int len = MultiByteToWideChar(CP_ACP, 0, str_gbk.c_str(), -1, NULL, 0);
	wchar_t* wszUTF8 = new wchar_t[len + 1];
	memset(wszUTF8, 0, len * 2 + 2);
	MultiByteToWideChar(CP_ACP, 0, str_gbk.c_str(), -1, wszUTF8, len);
	len = WideCharToMultiByte(CP_UTF8, 0, wszUTF8, -1, NULL, 0, NULL, NULL);
	char* szUTF8 = new char[len + 1];
	memset(szUTF8, 0, len + 1);
	WideCharToMultiByte(CP_UTF8, 0, wszUTF8, -1, szUTF8, len, NULL, NULL);
	std::string strTemp(szUTF8);
	if (szUTF8) delete[] szUTF8;
	if (wszUTF8) delete[] wszUTF8;
	return strTemp;
}

std::string _String::w_to_mulity_byte(std::wstring data)
{
	int size_needed = WideCharToMultiByte(CP_ACP, 0, &data[0], (int)data.size(), NULL, 0, NULL, NULL);
	std::string str(size_needed, 0);
	WideCharToMultiByte(CP_ACP, 0, &data[0], (int)data.size(), &str[0], size_needed, NULL, NULL);
	return str;
}

std::wstring _String::mulity_byte_to_w(std::string data)
{
	int size_needed = MultiByteToWideChar(CP_ACP, 0, &data[0], (int)data.size(), NULL, 0);
	std::wstring wstr(size_needed, 0);
	MultiByteToWideChar(CP_ACP, 0, &data[0], (int)data.size(), &wstr[0], size_needed);
	return wstr;
}