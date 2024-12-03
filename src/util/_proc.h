#pragma once

#include "macro.h"

#include <set>

#if defined(MSVC)
#define PID_TYPE unsigned long
#define PROC_TYPE wchar_t*
#elif defined(MINGW)
#define PID_TYPE unsigned long
#define PROC_TYPE char*
#elif defined(LINUX)
#include <signal.h>
#define PID_TYPE pid_t
#define PROC_TYPE char*
#endif

class CProcUtil
{
public:
	static std::set<PID_TYPE> get_proc_id_by_name(const PROC_TYPE proc_name);
	static bool close_process_by_name(const PROC_TYPE proc_name);
	static bool close_process_by_id(const PID_TYPE proc_id);
};