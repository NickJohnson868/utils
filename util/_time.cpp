#include "_time.h"

#include <windows.h>
#include <sstream>
#include <iomanip>
#include <iostream>

std::string _Time::get_cur_datetime(LPCSTR y_m_d, LPCSTR h_m_s)
{
	auto now = std::chrono::system_clock::now();
	std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
	std::tm local_tm = {};
	localtime_s(&local_tm, &time_t_now);

	std::stringstream ss;
	ss << std::setfill('0')
		<< std::setw(4) << (local_tm.tm_year + 1900) << y_m_d  // tm_year 从 1900 年开始
		<< std::setw(2) << (local_tm.tm_mon + 1) << y_m_d  // tm_mon 从 0 开始，1 月为 0
		<< std::setw(2) << local_tm.tm_mday << " "
		<< std::setw(2) << local_tm.tm_hour << h_m_s
		<< std::setw(2) << local_tm.tm_min << h_m_s
		<< std::setw(2) << local_tm.tm_sec;

	return ss.str();
}

std::string _Time::get_cur_datetime_ms(LPCSTR y_m_d, LPCSTR h_m_s)
{
	auto now = std::chrono::system_clock::now();
	// 获取毫秒部分
	auto duration = now.time_since_epoch();
	auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

	std::stringstream ss;
	ss << std::setfill('0') << get_cur_datetime() << "." << std::setw(3) << milliseconds;

	return ss.str();
}

std::string _Time::get_cur_datetime_us(LPCSTR y_m_d, LPCSTR h_m_s)
{
	auto now = std::chrono::system_clock::now();
	// 获取微秒部分
	auto duration = now.time_since_epoch();
	auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000000;

	std::stringstream ss;
	ss << std::setfill('0') << get_cur_datetime(y_m_d, h_m_s) << "." << std::setw(6) << microseconds;

	return ss.str();
}

std::string _Time::get_cur_datetime_ns(LPCSTR y_m_d, LPCSTR h_m_s)
{
	auto now = std::chrono::system_clock::now();
	// 获取微秒部分
	auto duration = now.time_since_epoch();
	auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() % 1000000000;

	std::stringstream ss;
	ss << std::setfill('0') << get_cur_datetime(y_m_d, h_m_s) << "." << std::setw(9) << nanoseconds;

	return ss.str();
}

int _Time::get_data_or_time(_TimeType type)
{
	auto now = std::chrono::system_clock::now();
	std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);
	std::tm local_tm = {};
	localtime_s(&local_tm, &time_t_now);

	switch (type)
	{
	case YEAR:
		return local_tm.tm_year + 1900;
	case MON:
		return local_tm.tm_mon + 1;
	case DAY:
		return local_tm.tm_mday;
	case HOUR:
		return local_tm.tm_hour;
	case MIN:
		return local_tm.tm_min;
	case SEC:
		return local_tm.tm_sec;
	default:
		return -1;
	}
}

long long _Time::get_timestamp(_StampType type)
{
	auto now = std::chrono::system_clock::now();  // 获取当前时间点
	long long timestamp = 0L;
	switch (type)
	{
	case NANO:
		timestamp = std::chrono::duration_cast<std::chrono::nanoseconds>(now.time_since_epoch()).count();
		break;
	case MICR:
		timestamp = std::chrono::duration_cast<std::chrono::microseconds>(now.time_since_epoch()).count();
		break;
	case MILL:
	default:
		timestamp = std::chrono::duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count();
		break;
	}
	
	return timestamp;
}

bool _Time::get_last_access_time(const std::filesystem::path& filePath, FileTime& filetime)
{
	try {
		HANDLE hFile = CreateFile(filePath.c_str(), GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);

		if (hFile == INVALID_HANDLE_VALUE)
		{
			std::cerr << "无法打开文件: " << filePath << std::endl;
			return FALSE;
		}
		else if (!GetFileTime(hFile, &filetime.creationTime, &filetime.lastAccessTime, &filetime.lastWriteTime))
		{
			std::cerr << "无法获取时间: " << filePath << std::endl;
			return FALSE;
		}

		CloseHandle(hFile);
		file_to_local_time(filetime.creationTime);
		file_to_local_time(filetime.lastAccessTime);
		file_to_local_time(filetime.lastWriteTime);
		return TRUE;
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return FALSE;
	}
}

void _Time::file_to_local_time(FILETIME& filetime)
{
	FILETIME backFiletime = filetime;
	FILETIME localFiletime;
	FileTimeToLocalFileTime(&backFiletime, &localFiletime);
	filetime = localFiletime;
}

std::string _Time::format_filetime(const FILETIME& filetime, LPCSTR y_m_d, LPCSTR h_m_s)
{
	try {
		SYSTEMTIME stFiletime;
		if (FileTimeToSystemTime(&filetime, &stFiletime))
		{
			std::ostringstream oss;
			oss << stFiletime.wYear << y_m_d
				<< stFiletime.wMonth << y_m_d
				<< stFiletime.wDay << " "
				<< stFiletime.wHour << h_m_s
				<< stFiletime.wMinute << h_m_s
				<< stFiletime.wSecond;
			return oss.str();
		}
	}
	catch (const std::exception& e)
	{
		std::cerr << e.what() << std::endl;
		return "";
	}
	return "";
}