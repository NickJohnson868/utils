#include "_time.h"
#include "util.h"

#include <sstream>
#include <iomanip>
#include <iostream>
#include <filesystem>

std::string CTimeUtil::get_cur_datetime(const char* y_m_d, const char* h_m_s) {
	try {
		// ��ȡ��ǰʱ��
		auto now = std::chrono::system_clock::now();
		std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);

		std::tm local_tm = {};
#ifdef WIN
		localtime_s(&local_tm, &time_t_now);
#elif LINUX
		localtime_r(&time_t_now, &local_tm);
#endif

		// ��ʽ��ʱ��
		std::stringstream ss;
		ss << std::setfill('0')
			<< std::setw(4) << (local_tm.tm_year + 1900) << y_m_d // tm_year �� 1900 �꿪ʼ
			<< std::setw(2) << (local_tm.tm_mon + 1) << y_m_d // tm_mon �� 0 ��ʼ��1 ��Ϊ 0
			<< std::setw(2) << local_tm.tm_mday << " "
			<< std::setw(2) << local_tm.tm_hour << h_m_s
			<< std::setw(2) << local_tm.tm_min << h_m_s
			<< std::setw(2) << local_tm.tm_sec;

		return ss.str();
	}
	catch (const std::exception& e) {
		std::cerr << "Error in get_cur_datetime: " << e.what() << std::endl;
		return "";
	}
}

std::string CTimeUtil::get_cur_datetime_ms(const char* y_m_d, const char* h_m_s) {
	try {
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration).count() % 1000;

		std::stringstream ss;
		ss << std::setfill('0') << get_cur_datetime(y_m_d, h_m_s) << "." << std::setw(3) << milliseconds;

		return ss.str();
	}
	catch (const std::exception& e) {
		Util::color_print(P_ERROR, "Error in get_cur_datetime_ms: %s\n", e.what());
		return "";
	}
}

std::string CTimeUtil::get_cur_datetime_us(const char* y_m_d, const char* h_m_s) {
	try {
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		auto microseconds = std::chrono::duration_cast<std::chrono::microseconds>(duration).count() % 1000000;

		std::stringstream ss;
		ss << std::setfill('0') << get_cur_datetime(y_m_d, h_m_s) << "." << std::setw(6) << microseconds;

		return ss.str();
	}
	catch (const std::exception& e) {
		Util::color_print(P_ERROR, "Error in get_cur_datetime_us: %s\n", e.what());
		return "";
	}
}

std::string CTimeUtil::get_cur_datetime_ns(const char* y_m_d, const char* h_m_s) {
	try {
		auto now = std::chrono::system_clock::now();
		auto duration = now.time_since_epoch();
		auto nanoseconds = std::chrono::duration_cast<std::chrono::nanoseconds>(duration).count() % 1000000000;

		std::stringstream ss;
		ss << std::setfill('0') << get_cur_datetime(y_m_d, h_m_s) << "." << std::setw(9) << nanoseconds;

		return ss.str();
	}
	catch (const std::exception& e) {
		Util::color_print(P_ERROR, "Error in get_cur_datetime_ns: %s\n", e.what());
		return "";
	}
}

int CTimeUtil::get_data_or_time(_TimeType type) {
	try {
		// ��ȡ��ǰʱ��
		auto now = std::chrono::system_clock::now();
		std::time_t time_t_now = std::chrono::system_clock::to_time_t(now);

		std::tm local_tm = {};
#ifdef WIN
		localtime_s(&local_tm, &time_t_now);
#elif LINUX
		localtime_r(&time_t_now, &local_tm);
#endif

		// ���� type ���ض�Ӧʱ�䲿��
		switch (type) {
		case YEAR:
			return local_tm.tm_year + 1900; // tm_year �Ǵ� 1900 �꿪ʼ����
		case MON:
			return local_tm.tm_mon + 1; // tm_mon �Ǵ� 0 ��ʼ������1 ���� 0
		case DAY:
			return local_tm.tm_mday;
		case HOUR:
			return local_tm.tm_hour;
		case MIN:
			return local_tm.tm_min;
		case SEC:
			return local_tm.tm_sec;
		default:
			return -1; // δ֪ type ���� -1
		}
	}
	catch (const std::exception& e) {
		std::cerr << "Error in get_data_or_time: " << e.what() << std::endl;
		return -1;
	}
}

long long CTimeUtil::get_timestamp(_StampType type) {
	try {
		auto now = std::chrono::system_clock::now(); // ��ȡ��ǰʱ���
		long long timestamp = 0L;
		switch (type) {
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
	catch (const std::exception& e) {
		Util::color_print(P_ERROR, "Error in get_timestamp: %s\n", e.what());
		return 0;
	}
}
