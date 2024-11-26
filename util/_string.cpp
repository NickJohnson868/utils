#pragma once

#include "_string.h"

#include <windows.h>
#include <sstream>
#include <random>
#include <iomanip>

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

bool _String::is_utf8(const std::string& str)
{
	int i = 0;
	while (i < str.size()) {
		unsigned char c = str[i];

		// 单字节字符（ASCII）
		if (c <= 0x7F) {
			i++;
		}
		// 两字节字符
		else if ((c & 0xE0) == 0xC0) {
			if (i + 1 >= str.size() || (str[i + 1] & 0xC0) != 0x80)
				return false;
			i += 2;
		}
		// 三字节字符
		else if ((c & 0xF0) == 0xE0) {
			if (i + 2 >= str.size() || (str[i + 1] & 0xC0) != 0x80 || (str[i + 2] & 0xC0) != 0x80)
				return false;
			i += 3;
		}
		// 四字节字符
		else if ((c & 0xF8) == 0xF0) {
			if (i + 3 >= str.size() || (str[i + 1] & 0xC0) != 0x80 || (str[i + 2] & 0xC0) != 0x80 || (str[i + 3] & 0xC0) != 0x80)
				return false;
			i += 4;
		}
		else {
			return false;
		}
	}
	return true;
}

bool _String::is_gbk(const std::string& str)
{
	int i = 0;
	while (i < str.size()) {
		unsigned char c = str[i];

		// ASCII 字符
		if (c <= 0x7F) {
			i++;
		}
		// 双字节字符 (GBK)
		else if ((c >= 0x81 && c <= 0xFE) && (i + 1 < str.size()) && (str[i + 1] >= 0x40 && str[i + 1] <= 0xFE)) {
			i += 2;
		}
		else {
			return false; // 非 GBK 合法字节
		}
	}
	return true;
}

std::string _String::url_encode(const std::string& value) 
{
	std::ostringstream encoded;
	for (char c : value) {
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~') {
			encoded << c;
		}
		else {
			encoded << '%' << std::uppercase << std::hex << int((unsigned char)c);
		}
	}
	return encoded.str();
}

std::string _String::url_decode(const std::string& value)
{
	std::ostringstream decoded;
	for (size_t i = 0; i < value.length(); ++i) {
		if (value[i] == '%') {
			int hex_value;
			std::istringstream(value.substr(i + 1, 2)) >> std::hex >> hex_value;
			decoded << static_cast<char>(hex_value);
			i += 2;
		}
		else if (value[i] == '+') {
			decoded << ' ';
		}
		else {
			decoded << value[i];
		}
	}
	return decoded.str();
}

std::vector<std::string> _String::split_str(const std::string& str, const std::string& delimiter)
{
	std::vector<std::string> result;
	size_t start = 0;
	size_t end = str.find(delimiter);
	if (str == "") return result;
	if (delimiter == "") end = std::string::npos;

	while (end != std::string::npos)
	{
		result.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}

	result.push_back(str.substr(start));

	return result;
}

std::string _String::generate_uuid(bool b_lower, bool b_delimiter)
{
	// 生成一个随机数引擎
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, 255); // 生成 0 到 255 的随机数

	// 生成一个 UUID v4 格式的 UUID
	unsigned char uuid[16];
	for (int i = 0; i < 16; ++i) {
		uuid[i] = dis(gen);
	}

	// 设置 UUID 的版本 (v4)
	uuid[6] = (uuid[6] & 0x0F) | 0x40;  // 设置版本为 4
	uuid[8] = (uuid[8] & 0x3F) | 0x80;  // 设置变体为 10xx

	// 将 UUID 转换为字符串
	std::stringstream ss;
	ss << std::setfill('0') << std::hex;

	// 拼接 UUID 格式 (8-4-4-4-12)
	ss << std::setw(2) << (int)uuid[0] << std::setw(2) << (int)uuid[1]
		<< std::setw(2) << (int)uuid[2] << std::setw(2) << (int)uuid[3];
	if (b_delimiter) ss << '-';
	ss << std::setw(2) << (int)uuid[4] << std::setw(2) << (int)uuid[5];
	if (b_delimiter) ss << '-';
	ss << std::setw(2) << (int)uuid[6] << std::setw(2) << (int)uuid[7];
	if (b_delimiter) ss << '-';
	ss << std::setw(2) << (int)uuid[8] << std::setw(2) << (int)uuid[9];
	if (b_delimiter) ss << '-';
	ss << std::setw(2) << (int)uuid[10] << std::setw(2) << (int)uuid[11]
		<< std::setw(2) << (int)uuid[12] << std::setw(2) << (int)uuid[13]
		<< std::setw(2) << (int)uuid[14] << std::setw(2) << (int)uuid[15];

	std::string uuid_str = ss.str();
	if (!b_lower) {
		to_upper(uuid_str);
	}

	return uuid_str;
}