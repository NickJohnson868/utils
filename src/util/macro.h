#pragma once

#if defined(__MINGW32__)
#define MINGW // mingw
#define WIN // windows
#if defined(__MINGW64__)
#define MINGW64
#else
#define MINGW32
#endif

#elif defined(_WIN32)
#define MSVC // MSVC
#define WIN // windows
#if defined(_WIN64)
#define WIN64
#else
#define WIN32
#endif

#elif defined(__linux__)
#define LINUX
#if defined(__x86_64__)
#define LINUX64
#elif defined(__i386__)
#define LINUX32
#endif

#else
#define UNKNOWN_PLATFORM
#endif

#ifdef WIN
#define FILE_SEP "\\"
#define FFMPEG_DIR (CFileUtil::get_exe_dir().string() + FILE_SEP + "ffmpeg\\").data()
#define OPEN_MODE "rb"
#elif defined(LINUX)
#define _popen popen
#define _pclose pclose
#define sscanf_s sscanf
#define FILE_SEP "/"
#define FFMPEG_DIR (CFileUtil::get_exe_dir().string() + FILE_SEP + "ffmpeg/").data()
#define OPEN_MODE "r"
#endif