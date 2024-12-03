#include <iostream>
#include <filesystem>

#include "pixel_on_cmd.h"
#include "image.hpp"

#ifdef WIN
#include <windows.h>
#elif defined(LINUX)
#include <csignal>
#include <unistd.h>
#endif

using namespace std;

CVideoPlayer* video = nullptr;

void close_player()
{
    if (video)
    {
        delete video;
        video = nullptr;
    }
}

#ifdef WIN
BOOL WINAPI signal_handler(DWORD dwCtrlType)
{
    switch (dwCtrlType)
    {
    case CTRL_CLOSE_EVENT:
        printf("\033c");
        close_player();
        reset_screen(192, 20);
        return true;
    default:
        return false;
    }
    return false;
}
#elif defined(LINUX)
void signal_handler(int signal) {
	if (signal == SIGINT || signal == SIGTERM) {
		printf("\033c");		// ANSI 清屏指令
		close_player();
		reset_screen(192, 20); // 自定义重置屏幕功能
		exit(0);               // 终止程序
	}
}
#endif

bool is_gif(const string& extension)
{
    return extension == ".gif";
}

bool is_image(const string& extension)
{
    return (
        extension == ".png" || extension == ".jpg" ||
        extension == ".jpeg"
    );
}

bool is_video(const string& extension)
{
    return (
        extension == ".mp4" || extension == ".avi" ||
        extension == ".mkv" || extension == ".flv"
    );
}

void play_video(std::filesystem::path file, int maxSize = 192, int font = 16)
{
    video = new CVideoPlayer;
    _Type t;
    if (is_image(file.extension().string())) t = _IMAGE;
    else if (is_gif(file.extension().string())) t = _GIF;
    else if (is_video(file.extension().string())) t = _VIDEO;
    else return;

    video->read_video(file, t, maxSize);
    printf("\033c");
    reset_screen(maxSize, font);
    video->display();
    close_player();
}

namespace PixelOnCmd
{
    void run(int argc, char* argv[])
    {
        if (enable_vt_mode() < 0)
        {
            std::cerr << "终端不支持ANSI转义序列." << std::endl;
            return;
        }

#ifdef WIN
        if (!SetConsoleCtrlHandler(signal_handler, TRUE))
        {
            std::cerr << "无法设置控制处理器: " << GetLastError() << std::endl;
            return;
        }
#elif defined(LINUX)
		// 设置信号处理函数
		signal(SIGINT, signal_handler);
		signal(SIGTERM, signal_handler);
#endif

        if (argc <= 1)
        {
            string file = CStringUtil::format("%s%s%s%s%s", CFileUtil::get_project_dir().string().data(),
                                              FILE_SEP, "res", FILE_SEP, "24fps.mp4");
            cout << file << endl;
            play_video(file, 256, 1);
        }
        else
            play_video(argv[1], 256, 1);
    }
}
