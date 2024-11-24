#include "_time.h"

#include <windows.h>
#include <sstream>
#include <iomanip>
#include <iostream>

std::string _Time::get_cur_datetime()
{
	SYSTEMTIME sysTime;
	GetLocalTime(&sysTime);
	std::stringstream ss;
	ss << std::setfill('0')
		<< std::setw(4) << sysTime.wYear << "/"
		<< std::setw(2) << sysTime.wMonth << "/"
		<< std::setw(2) << sysTime.wDay << " "
		<< std::setw(2) << sysTime.wHour << ":"
		<< std::setw(2) << sysTime.wMinute << ":"
		<< std::setw(2) << sysTime.wSecond;
	return ss.str();
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