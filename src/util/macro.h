#pragma once

#ifdef _WIN32
#define ffmpeg std::string("ffmpeg\\")
#define OPEN_MODE "rb"
#define FILE_SEP "\\"
#elif __linux__
#define _popen popen
#define _pclose pclose
#define sscanf_s sscanf
#define ffmpeg std::string("ffmpeg/")
#define OPEN_MODE "r"
#define FILE_SEP "/"
#endif