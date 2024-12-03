#pragma once
#include "macro.h"

#include <string>
#include <algorithm>
#include <vector>
#include <cctype>
#include <cwctype>

#define CHECK_TEMP_STRING	std::is_same<StringT, std::string>::value
#define CHECK_TEMP_WSTRING	std::is_same<StringT, std::wstring>::value

#define CP_GBK 936

#ifdef LINUX
#define CP_UTF8 65001
#endif

using UINT = unsigned int;

class CStringUtil
{
public:
	/* CP_ACP CP_UTF8 */
	static std::string wstring_to_multibyte(const std::wstring& data, UINT codePage);

	static std::wstring multibyte_to_wstring(const std::string& data, UINT codePage);

	static std::string gbk_to_utf8(const std::string& str_gbk);

	static std::string utf8_to_gbk(const std::string& str_utf8);

	static bool is_utf8(const std::string& str);

	static bool is_gbk(const std::string& str);

	static std::string url_encode(const std::string& value);

	static std::string url_decode(const std::string& value);

	static std::vector<std::string> split_str(const std::string& str, const std::string& delimiter);

	static std::string generate_uuid(bool b_lower = true, bool b_delimiter = true);

	static bool is_empty(const std::string& str);

	static bool starts_with(const std::string& str, const std::string& prefix);

	static bool ends_with(const std::string& str, const std::string& suffix);

	static std::string trim(const std::string& str);

	static std::string double_to_string(double value, int precision = 2);

	static std::string format(const char* format, ...);

	static std::string reverse(const std::string& str);

	static std::string wildcard_to_regex(const std::string& wildcard);

	static bool match_wildcard(const std::string& text, const std::string& wildcard);

	static bool match_regex(const std::string& text, const std::string& regex_str);

	template <typename StringT>
	static StringT replace_all(const StringT& src, const StringT& target, const StringT& dest)
	{
		static_assert(CHECK_TEMP_STRING || CHECK_TEMP_WSTRING,
			"Invalid type for replace_all, only std::string and std::wstring are supported");

		StringT str(src);
		size_t	start_pos = 0;
		while ((start_pos = str.find(target, start_pos)) != StringT::npos)
		{
			str.replace(start_pos, target.length(), dest);
			start_pos += dest.length();
		}
		return str;
	}


	template <typename StringT>
	static bool compare_ignore_case(StringT str1, StringT str2)
	{
		static_assert(CHECK_TEMP_STRING || CHECK_TEMP_WSTRING,
			"Invalid type for replace_all, only std::string and std::wstring are supported");

		if (CHECK_TEMP_STRING)
		{
			std::transform(str1.begin(), str1.end(), str1.begin(), [](unsigned char c) { return toupper(c); });
			std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c) { return toupper(c); });
		}
		else {
			std::transform(str1.begin(), str1.end(), str1.begin(), [](unsigned char c) { return towupper(c); });
			std::transform(str2.begin(), str2.end(), str2.begin(), [](unsigned char c) { return towupper(c); });
		}
		return str1 == str2;
	}


	template <typename StringT>
	static void to_lower(StringT& str)
	{
		static_assert(CHECK_TEMP_STRING || CHECK_TEMP_WSTRING,
			"Invalid type for replace_all, only std::string and std::wstring are supported");

		if (CHECK_TEMP_STRING)
		{
			std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return toupper(c); });
		}
		else {
			std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return towupper(c); });
		}
	}


	template <typename StringT>
	static void to_upper(StringT& str)
	{
		static_assert(CHECK_TEMP_STRING || CHECK_TEMP_WSTRING,
			"Invalid type for replace_all, only std::string and std::wstring are supported");

		if (CHECK_TEMP_STRING)
		{
			std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return toupper(c); });
		}
		else {
			std::transform(str.begin(), str.end(), str.begin(), [](unsigned char c) { return towupper(c); });
		}
	}
};