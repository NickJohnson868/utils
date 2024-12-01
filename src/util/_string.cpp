#include <sstream>
#include <random>
#include <iomanip>
#include <iostream>
#include <regex>
#include <cstdarg>

#include "_string.h"

#ifdef _WIN32
#include <windows.h>
#elif __linux__
#include <iconv.h>
#define CP_TO(x) x==CP_GBK?("GBK"):(x==CP_UTF8?"UTF-8":"")
#endif

/* UTF-8 转 GBK */
std::string CStringUtil::utf8_to_gbk(const std::string& str_utf8)
{
	if (str_utf8.empty()) {
		return "";
	}
#ifdef _WIN32
	/* Step 1: UTF-8 转 WideChar (Unicode) */
	int wideCharSize = MultiByteToWideChar(CP_UTF8, 0, str_utf8.c_str(), static_cast<int>(str_utf8.size()), nullptr, 0);
	if (wideCharSize == 0)
		return "";

	std::wstring wideStr(wideCharSize, 0);
	MultiByteToWideChar(CP_UTF8, 0, str_utf8.c_str(), static_cast<int>(str_utf8.size()), &wideStr[0], wideCharSize);

	/* Step 2: WideChar 转 GBK (ANSI) */
	int gbkSize = WideCharToMultiByte(CP_GBK, 0, wideStr.c_str(), wideCharSize, nullptr, 0, nullptr, nullptr);
	if (gbkSize == 0)
		return "";

	std::string gbkStr(gbkSize, 0);
	WideCharToMultiByte(CP_GBK, 0, wideStr.c_str(), wideCharSize, &gbkStr[0], gbkSize, nullptr, nullptr);

	return gbkStr;
#elif __linux__
	// 创建 iconv 转换描述符
	iconv_t cd = iconv_open("GBK", "UTF-8");
	if (cd == (iconv_t)-1) {
		throw std::runtime_error("iconv_open failed");
	}

	size_t inBytesLeft = str_utf8.size();
	size_t outBytesLeft = inBytesLeft * 2; // GBK 编码通常比 UTF-8 更短

	// 分配输出缓冲区
	std::vector<char> outBuffer(outBytesLeft);
	char* inBuf = const_cast<char*>(str_utf8.data());
	char* outBuf = outBuffer.data();

	// 执行转换
	if (iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft) == (size_t)-1) {
		iconv_close(cd);
		throw std::runtime_error("iconv conversion failed");
	}

	// 关闭 iconv 描述符
	iconv_close(cd);

	// 返回转换后的字符串
	return std::string(outBuffer.data(), outBuffer.size() - outBytesLeft);
#endif
}


/* GBK 转 UTF-8 */
std::string CStringUtil::gbk_to_utf8(const std::string& str_gbk)
{
	if (str_gbk.empty())
		return "";

#ifdef _WIN32
	/* Step 1: GBK 转 WideChar (Unicode) */
	int wideCharSize = MultiByteToWideChar(CP_GBK, 0, str_gbk.c_str(), static_cast<int>(str_gbk.size()), nullptr, 0);
	if (wideCharSize == 0)
		return "";

	std::wstring wideStr(wideCharSize, 0);
	MultiByteToWideChar(CP_GBK, 0, str_gbk.c_str(), static_cast<int>(str_gbk.size()), &wideStr[0], wideCharSize);

	/* Step 2: WideChar 转 UTF-8 */
	int utf8Size = WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), wideCharSize, nullptr, 0, nullptr, nullptr);
	if (utf8Size == 0)
		return "";

	std::string utf8Str(utf8Size, 0);
	WideCharToMultiByte(CP_UTF8, 0, wideStr.c_str(), wideCharSize, &utf8Str[0], utf8Size, nullptr, nullptr);

	return utf8Str;
#elif __linux__
	// 创建 iconv 转换描述符
	iconv_t cd = iconv_open("UTF-8", "GBK");
	if (cd == (iconv_t)-1) {
		throw std::runtime_error("iconv_open failed");
	}

	size_t inBytesLeft = str_gbk.size();
	size_t outBytesLeft = inBytesLeft * 2; // UTF-8 通常比 GBK 更长

	// 分配输出缓冲区
	std::vector<char> outBuffer(outBytesLeft);
	char* inBuf = const_cast<char*>(str_gbk.data());
	char* outBuf = outBuffer.data();

	// 执行转换
	if (iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft) == (size_t)-1) {
		iconv_close(cd);
		throw std::runtime_error("iconv conversion failed");
	}

	// 关闭 iconv 描述符
	iconv_close(cd);

	// 返回转换后的字符串
	return std::string(outBuffer.data(), outBuffer.size() - outBytesLeft);
#endif
}


std::string CStringUtil::wstring_to_multibyte(const std::wstring& wstr, UINT codePage)
{
	if (wstr.empty())
		return "";

#ifdef _WIN32
	int sizeNeeded = WideCharToMultiByte(
		codePage,                       /* 代码页，例如 CP_UTF8 或 CP_ACP */
		0,                              /* 转换标志 */
		wstr.c_str(),                   /* 输入宽字符字符串 */
		static_cast<int>(wstr.size()), /* 字符数 */
		nullptr,                        /* 输出缓冲区 */
		0,                              /* 缓冲区大小，0 表示获取所需大小 */
		nullptr,                        /* 无默认字符 */
		nullptr                         /* 不检查默认字符是否被使用 */
	);

	std::string result(sizeNeeded, 0);
	WideCharToMultiByte(
		codePage, 0, wstr.c_str(), static_cast<int>(wstr.size()),
		&result[0], sizeNeeded, nullptr, nullptr
	);

	return result;
#elif __linux__
	// 创建 iconv 转换描述符
	iconv_t cd = iconv_open(CP_TO(codePage), "WCHAR_T");
	if (cd == (iconv_t)-1) {
		throw std::runtime_error("iconv_open failed");
	}

	// 转换
	size_t inBytesLeft = wstr.size() * sizeof(wchar_t);
	size_t outBytesLeft = inBytesLeft * 2; // 输出缓冲区，假设多字节编码比宽字符编码更长

	std::vector<char> outBuffer(outBytesLeft);
	char* inBuf = reinterpret_cast<char*>(const_cast<wchar_t*>(wstr.data()));
	char* outBuf = outBuffer.data();

	if (iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft) == (size_t)-1) {
		iconv_close(cd);
		throw std::runtime_error("iconv conversion failed");
	}

	iconv_close(cd);
	return std::string(outBuffer.data(), outBuffer.size() - outBytesLeft);
#endif
}


std::wstring CStringUtil::multibyte_to_wstring(const std::string& str, UINT codePage)
{
	if (str.empty())
		return L"";

#ifdef _WIN32
	int sizeNeeded = MultiByteToWideChar(
		codePage,                       /* 代码页，例如 CP_UTF8 或 CP_ACP */
		0,                              /* 转换标志 */
		str.c_str(),                    /* 输入的多字节字符串 */
		static_cast<int>(str.size()),  /* 字符数 */
		nullptr,                        /* 输出缓冲区 */
		0                               /* 缓冲区大小，0 表示获取所需大小 */
	);

	std::wstring result(sizeNeeded, 0);
	MultiByteToWideChar(
		codePage, 0, str.c_str(), static_cast<int>(str.size()),
		&result[0], sizeNeeded
	);

	return result;
#elif __linux__
	// 创建 iconv 转换描述符
	iconv_t cd = iconv_open("WCHAR_T", CP_TO(codePage));
	if (cd == (iconv_t)-1) {
		throw std::runtime_error("iconv_open failed");
	}

	// 转换
	size_t inBytesLeft = str.size();
	size_t outBytesLeft = inBytesLeft * sizeof(wchar_t); // 假设宽字符比多字节字符更长

	std::vector<char> outBuffer(outBytesLeft);
	char* inBuf = const_cast<char*>(str.data());
	char* outBuf = outBuffer.data();

	if (iconv(cd, &inBuf, &inBytesLeft, &outBuf, &outBytesLeft) == (size_t)-1) {
		iconv_close(cd);
		throw std::runtime_error("iconv conversion failed");
	}

	iconv_close(cd);
	return std::wstring(reinterpret_cast<wchar_t*>(outBuffer.data()), (outBuffer.size() - outBytesLeft) / sizeof(wchar_t));
#endif
}


bool CStringUtil::is_utf8(const std::string& str)
{
	int c, i, ix, n, j;
	for (i = 0, ix = str.length(); i < ix; i++)
	{
		c = (unsigned char)str[i];

		/* 0BBBBBBB: 单字节字符 */
		if (0x00 <= c && c <= 0x7f)
			n = 0;
		/* 110BBBBB: 双字节字符（UTF-8） */
		else if ((c & 0xE0) == 0xC0)
			n = 1;
		/* 检查非法代理对（U+D800 到 U+DFFF） */
		else if (c == 0xed && i < (ix - 1) && ((unsigned char)str[i + 1] & 0xa0) == 0xa0)
			return false;  /* Illegal surrogate pair in UTF-8 (U+D800-U+DFFF) */
		/* 1110BBBB: 三字节字符（UTF-8） */
		else if ((c & 0xF0) == 0xE0)
			n = 2;
		/* 11110BBB: 四字节字符（UTF-8） */
		else if ((c & 0xF8) == 0xF0)
			n = 3;
		/* 非法字符：不是有效的UTF-8字节序列 */
		else return false;

		/* 检查后续字节 */
		for (j = 0; j < n && i < ix; j++)
		{
			if ((++i == ix) || (((unsigned char)str[i] & 0xC0) != 0x80))
				return false;  /* 后续字节必须以 `10bbbbbb` 开头 */
		}
	}

	return true;
}


bool CStringUtil::is_gbk(const std::string& str)
{
	size_t length = str.length();
	for (size_t i = 0; i < length; ++i)
	{
		unsigned char c = static_cast<unsigned char>(str[i]);

		/* 单字节字符（ASCII字符：0x00 - 0x7F） */
		if (c <= 0x7F)
		{
			continue;
		}

		/* 双字节字符（GBK编码范围：高字节 0x81 - 0xFE, 低字节 0x40 - 0x7E 或 0x80 - 0xFE） */
		if (i + 1 < length)
		{
			unsigned char next_c = (unsigned char)str[i + 1];
			if (c >= 0x81 && c <= 0xFE)
			{
				if ((next_c >= 0x40 && next_c <= 0x7E) || (next_c >= 0x80 && next_c <= 0xFE))
				{
					i++; /* 跳过当前字符的下一个字节 */
					continue;
				}
			}
		}

		/* 如果不符合 GBK 编码的规则，返回 false */
		return false;
	}

	return true;
}


std::string CStringUtil::url_encode(const std::string& value)
{
	std::ostringstream encoded;
	for (char c : value)
	{
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~')
		{
			encoded << c;
		}
		else {
			encoded << '%' << std::uppercase << std::hex << int((unsigned char)c);
		}
	}
	return encoded.str();
}


std::string CStringUtil::url_decode(const std::string& value)
{
	std::ostringstream decoded;
	for (size_t i = 0; i < value.length(); ++i)
	{
		if (value[i] == '%')
		{
			int hex_value;
			std::istringstream(value.substr(i + 1, 2)) >> std::hex >> hex_value;
			decoded << static_cast<char>(hex_value);
			i += 2;
		}
		else if (value[i] == '+')
		{
			decoded << ' ';
		}
		else {
			decoded << value[i];
		}
	}
	return decoded.str();
}


std::vector<std::string> CStringUtil::split_str(const std::string& str, const std::string& delimiter)
{
	std::vector<std::string>	result;
	size_t				start = 0;
	size_t				end = str.find(delimiter);
	if (str == "")
		return result;
	if (delimiter == "")
		end = std::string::npos;

	while (end != std::string::npos)
	{
		result.push_back(str.substr(start, end - start));
		start = end + delimiter.length();
		end = str.find(delimiter, start);
	}

	result.push_back(str.substr(start));

	return result;
}


std::string CStringUtil::generate_uuid(bool b_lower, bool b_delimiter)
{
	/* 生成一个随机数引擎 */
	std::random_device		rd;
	std::mt19937			gen(rd());
	std::uniform_int_distribution<> dis(0, 255); /* 生成 0 到 255 的随机数 */

	/* 生成一个 UUID v4 格式的 UUID */
	unsigned char uuid[16];
	for (int i = 0; i < 16; ++i)
	{
		uuid[i] = dis(gen);
	}

	/* 设置 UUID 的版本 (v4) */
	uuid[6] = (uuid[6] & 0x0F) | 0x40;      /* 设置版本为 4 */
	uuid[8] = (uuid[8] & 0x3F) | 0x80;      /* 设置变体为 10xx */

	/* 将 UUID 转换为字符串 */
	std::stringstream ss;
	ss << std::setfill('0') << std::hex;

	/* 拼接 UUID 格式 (8-4-4-4-12) */
	ss << std::setw(2) << (int)uuid[0] << std::setw(2) << (int)uuid[1]
		<< std::setw(2) << (int)uuid[2] << std::setw(2) << (int)uuid[3];
	if (b_delimiter)
		ss << '-';
	ss << std::setw(2) << (int)uuid[4] << std::setw(2) << (int)uuid[5];
	if (b_delimiter)
		ss << '-';
	ss << std::setw(2) << (int)uuid[6] << std::setw(2) << (int)uuid[7];
	if (b_delimiter)
		ss << '-';
	ss << std::setw(2) << (int)uuid[8] << std::setw(2) << (int)uuid[9];
	if (b_delimiter)
		ss << '-';
	ss << std::setw(2) << (int)uuid[10] << std::setw(2) << (int)uuid[11]
		<< std::setw(2) << (int)uuid[12] << std::setw(2) << (int)uuid[13]
		<< std::setw(2) << (int)uuid[14] << std::setw(2) << (int)uuid[15];

	std::string uuid_str = ss.str();
	if (!b_lower)
	{
		to_upper(uuid_str);
	}

	return uuid_str;
}


bool CStringUtil::is_empty(const std::string& str)
{
	return str.find_first_not_of(" \t\r\n") == std::string::npos;
}


bool CStringUtil::starts_with(const std::string& str, const std::string& prefix)
{
	return str.rfind(prefix, 0) == 0;
}


bool CStringUtil::ends_with(const std::string& str, const std::string& suffix)
{
	return (str.length() >= suffix.length()) && (str.compare(str.length() - suffix.length(), suffix.length(), suffix) == 0);
}


std::string CStringUtil::trim(const std::string& str)
{
	auto	start = str.find_first_not_of(" \t\r\n");
	auto	end = str.find_last_not_of(" \t\r\n");
	return (start == std::string::npos || end == std::string::npos) ? "" : str.substr(start, end - start + 1);
}


std::string CStringUtil::double_to_string(double value, int precision)
{
	std::ostringstream oss;
	oss << std::fixed << std::setprecision(precision) << value;
	return oss.str();
}


std::string CStringUtil::format(const char* format, ...)
{
	char	buffer[1024];
	va_list args;
	va_start(args, format);
	vsnprintf(buffer, sizeof(buffer), format, args);
	va_end(args);
	return std::string(buffer);
}


std::string CStringUtil::reverse(const std::string& str)
{
	return std::string(str.rbegin(), str.rend());
}

std::string CStringUtil::wildcard_to_regex(const std::string& wildcard)
{
	const std::string special_chars = R"(\^.$|()[]+{})";
	std::string regex = "^";

	for (char c : wildcard) {
		if (c == '*') {
			regex += ".*";
		}
		else if (c == '?') {
			regex += ".";
		}
		else {
			// 如果是特殊字符，则加反斜杠转义
			if (special_chars.find(c) != std::string::npos) {
				regex += '\\';
			}
			regex += c;
		}
	}

	regex += "$";
	return regex;
}

bool CStringUtil::match_wildcard(const std::string& text, const std::string& wildcard)
{
	if (is_empty(wildcard)) return false;
	std::string regexPattern = wildcard_to_regex(wildcard);
	return match_regex(text, regexPattern);
}

bool CStringUtil::match_regex(const std::string& text, const std::string& regex_str)
{
	if (is_empty(regex_str)) return false;
	std::regex regex(regex_str);
	return std::regex_match(text, regex);
}