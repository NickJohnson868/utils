#pragma once

#include <string>
#include <algorithm>

#define CHECK_TEMP_STRING	std::is_same<StringT, std::string>::value
#define CHECK_TEMP_WSTRING	std::is_same<StringT, std::wstring>::value

class _String
{
public:
	static std::string w_to_mulity_byte(std::wstring);
	static std::wstring mulity_byte_to_w(std::string);

	static std::string gbk_to_utf8(const std::string& str_gbk);
	static std::string utf8_to_gbk(const std::string& str_utf8);

	static bool is_utf8(const std::string& str);
	static bool is_gbk(const std::string& str);

	template <typename StringT>
	static StringT replace_all(StringT str, const StringT& target, const StringT& dest)
	{
		static_assert(CHECK_TEMP_STRING || CHECK_TEMP_WSTRING,
			"Invalid type for replace_all, only std::string and std::wstring are supported");

		size_t start_pos = 0;
		while ((start_pos = str.find(target, start_pos)) != StringT::npos) {
			str.replace(start_pos, target.length(), dest);
			start_pos += dest.length();
		}
		return str;
	}

	template <typename StringT>
	static bool compare_ignore_case(StringT& str1, StringT& str2)
	{
		static_assert(CHECK_TEMP_STRING || CHECK_TEMP_WSTRING,
			"Invalid type for replace_all, only std::string and std::wstring are supported");

		if (CHECK_TEMP_STRING) {
			std::transform(str1.begin(), str1.end(), str1.begin(), toupper);
			std::transform(str2.begin(), str2.end(), str2.begin(), toupper);
		}
		else {
			std::transform(str1.begin(), str1.end(), str1.begin(), towupper);
			std::transform(str2.begin(), str2.end(), str2.begin(), towupper);
		}
		return str1 == str2;
	}
};