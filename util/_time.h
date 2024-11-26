#pragma once

#include <string>
#include <filesystem>
#include <windows.h>
#include <functional>
#include <type_traits>
#include <chrono>

enum _TimeType
{
	YEAR,
	MON,
	DAY,
	HOUR,
	MIN,
	SEC
};

enum _StampType
{
	NANO,
	MICR,
	MILL,
};

typedef struct __FILETIME__
{
	FILETIME creationTime, lastWriteTime, lastAccessTime;
} FileTime;

template<typename T>
struct FunctionResult {
	T result;
	std::chrono::microseconds duration;
};

template<>
struct FunctionResult<void> {
	std::chrono::microseconds duration;
};

class _Time
{
public:
	static std::string get_cur_datetime(LPCSTR y_m_d = "-", LPCSTR h_m_s = ":");
	static std::string get_cur_datetime_ms(LPCSTR y_m_d = "-", LPCSTR h_m_s = ":");
	static std::string get_cur_datetime_us(LPCSTR y_m_d = "-", LPCSTR h_m_s = ":");
	static std::string get_cur_datetime_ns(LPCSTR y_m_d = "-", LPCSTR h_m_s = ":");
	static int get_data_or_time(_TimeType type);
	static long long get_timestamp(_StampType type = MILL);

	static std::string format_filetime(const FILETIME& filetime, LPCSTR y_m_d = "/", LPCSTR h_m_s = ":");
	static bool get_last_access_time(const std::filesystem::path& filePath, FileTime& lastAccessTime);
	static void file_to_local_time(FILETIME& filetime);

	template<typename Func, typename... Args>
	static auto time_execution(Func func, Args... args) -> FunctionResult<decltype(func(args...))>
	{
		if constexpr (std::is_void_v<decltype(func(args...))>)
		{
			auto start = std::chrono::high_resolution_clock::now();
			func(args...);
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = duration_cast<std::chrono::microseconds>(stop - start);
			return { duration };
		}
		else
		{
			auto start = std::chrono::high_resolution_clock::now();
			auto result = func(args...);
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = duration_cast<std::chrono::microseconds>(stop - start);
			return { result, duration };
		}
	}
};