#pragma once

#include <set>

#ifdef _WIN32
#define PID_TYPE unsigned long
#ifdef __MINGW32__
#define PROC_TYPE char*
#else
#define PROC_TYPE wchar_t*
#endif
#elif __linux__
#include <signal.h>
#define PID_TYPE pid_t
#define PROC_TYPE char*
#endif

class CProcUtil
{
public:
	static std::set<PID_TYPE> get_proc_id_by_name(const PROC_TYPE proc_name);
	static void close_process_by_name(const PROC_TYPE proc_name);
};