#pragma once

#include <string>
#include <filesystem>
#include <type_traits>
#include <chrono>

enum _TimeType {
	YEAR,
	MON,
	DAY,
	HOUR,
	MIN,
	SEC
};

enum _StampType {
	NANO,
	MICR,
	MILL,
};

template<typename T>
struct FunctionResult {
	T result;
	std::chrono::microseconds duration;
};

template<>
struct FunctionResult<void> {
	std::chrono::microseconds duration;
};

class CTimeUtil {
public:
	static std::string get_cur_datetime(const char* y_m_d = "-", const char* h_m_s = ":");

	static std::string get_cur_datetime_ms(const char* y_m_d = "-", const char* h_m_s = ":");

	static std::string get_cur_datetime_us(const char* y_m_d = "-", const char* h_m_s = ":");

	static std::string get_cur_datetime_ns(const char* y_m_d = "-", const char* h_m_s = ":");

	static int get_data_or_time(_TimeType type);

	static long long get_timestamp(_StampType type = MILL);

	template<typename Func, typename... Args>
	static auto time_execution(Func func, Args... args) -> FunctionResult<decltype(func(args...))> {
		if constexpr (std::is_void_v<decltype(func(args...))>) {
			auto start = std::chrono::high_resolution_clock::now();
			func(args...);
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = duration_cast<std::chrono::microseconds>(stop - start);
			return { duration };
		}
		else {
			auto start = std::chrono::high_resolution_clock::now();
			auto result = func(args...);
			auto stop = std::chrono::high_resolution_clock::now();
			auto duration = duration_cast<std::chrono::microseconds>(stop - start);
			return { result, duration };
		}
	}
};
