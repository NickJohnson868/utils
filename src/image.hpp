#pragma once
#include "util/macro.h"

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#ifndef __MINGW32__
#pragma comment(lib, "winmm.lib")
#endif
#elif __linux__
#include <unistd.h>
#endif

#include <filesystem>
#include <stdexcept>
#include <sstream>
#include <string>
#include <vector>
#include "util/_file.h"

#include <iostream>
#include <cstdlib>
#include "util/util.h"
#include "util/_string.h"

#define CHARACTER "#"
namespace fs = std::filesystem;
void reset_screen(int max_size = 192, int font_size = 8);
int enable_vt_mode();

struct Pixel
{
    unsigned char R, G, B;
};

enum _Type
{
    _IMAGE,
    _VIDEO,
    _GIF
};

class CFrame
{
public:
    CFrame()
    {
    }

    virtual ~CFrame()
    {
    }

    void read_rgba(std::vector<uint8_t>& rgba_data, int width, int height)
    {
        std::ostringstream ss;
        ss.str().reserve(width * height);
        ss << "\033[H";

        const std::string color_prefix = "\033[38;2;";
        const std::string color_suffix = "m";
        const std::string reset_color = "\033[0m";

        for (int y = 0; y < height; ++y)
        {
            for (int x = 0; x < width; ++x)
            {
                int index = (y * width + x) * 4;
                uint8_t R = rgba_data[index];
                uint8_t G = rgba_data[index + 1];
                uint8_t B = rgba_data[index + 2];
                uint8_t A = rgba_data[index + 3];

                RGB rgb = Util::rgba_to_rgb({R, G, B, A}, {12, 12, 12});
                R = rgb.R;
                G = rgb.G;
                B = rgb.B;

                ss << color_prefix << static_cast<int>(R) << ";"
                    << static_cast<int>(G) << ";" << static_cast<int>(B)
                    << color_suffix << CHARACTER;
            }
            ss << '\n';
        }
        ss << reset_color;
        data = ss.str();
    }


    void display()
    {
        printf("%s", data.data());
        fflush(stdout);
    }

private:
    std::string data;
    int m_width, m_height;
};

class CVideoPlayer
{
public:
    CVideoPlayer() : m_frameWidth(0), m_frameHeight(0), m_fps(0)
    {
        sound_playing = 0;
    }

    virtual ~CVideoPlayer()
    {
        stop_sound();
        if (std::filesystem::exists(m_audio_path))
        {
            std::filesystem::remove(m_audio_path);
        }
    }


    void display()
    {
        bool cursor_visible = Util::is_cursor_visible();
        Util::set_cursor_visible(false);
        play_sound();
        play_frame();
        stop_sound();
        Util::set_cursor_visible(cursor_visible);
    }


    void read_video(const std::filesystem::path& videoPath, _Type type, int maxSize = 192)
    {
        CVideoInfo video(videoPath);
        m_frameWidth = video.get_width();
        m_frameHeight = video.get_height();
        std::cout << "压缩文件..." << std::endl;
        fs::path com_path;
        if (type == _VIDEO || type == _GIF)
        {
            m_fps = video.get_fps();
            m_maxFrames = video.get_frames();
            com_path = CFileUtil::compress_video(videoPath, maxSize, 1.0, 0.5, m_frameWidth, m_frameHeight);
            if (type == _VIDEO) m_audio_path = extract_audio(videoPath);
        }
        else if (type == _IMAGE)
        {
            m_fps = 1;
            m_maxFrames = 1;
            com_path = CFileUtil::compress_image(videoPath, maxSize, 1.0, 0.5, m_frameWidth, m_frameHeight);
        }
        else
        {
            return;
        }

        int frameCount = 0;
        /* 加入 -vsync 0 和 -f image2pipe，确保同步 */
        std::string command = ffmpeg + "ffmpeg -nostats -loglevel error -hide_banner -i \""
            + com_path.string()
            + "\" -f image2pipe -pix_fmt rgba -vcodec rawvideo -vsync 0 -";

        std::cout << command << std::endl;
        FILE* pipe = _popen(command.c_str(), OPEN_MODE);
        if (!pipe)
            throw std::runtime_error("ffmpeg command failed");

        std::cout << "读取视频帧数据..." << std::endl;
        bool cursor_visible = Util::is_cursor_visible();
        Util::set_cursor_visible(false);
        while (frameCount < m_maxFrames)
        {
            Util::print_progress_bar(frameCount, m_maxFrames);
            CFrame image;

            std::vector<uint8_t> rgb_data(m_frameWidth * m_frameHeight * 4);
            size_t bytesRead = fread(rgb_data.data(), 1, rgb_data.size(), pipe);
            if (bytesRead != rgb_data.size())
                break; /* EOF or error */

            image.read_rgba(rgb_data, m_frameWidth, m_frameHeight);
            m_frames.push_back(image);

            frameCount++;
        }
        _pclose(pipe);
        Util::print_progress_bar(m_maxFrames, m_maxFrames);
        printf("\n");
        Util::set_cursor_visible(cursor_visible);
        CFileUtil::delete_file(com_path);
    }

public:
    bool sound_playing;

private:
    std::vector<CFrame> m_frames;
    int m_frameWidth, m_frameHeight, m_maxFrames;
    double m_fps;
    std::filesystem::path m_audio_path;

    void play_frame()
    {
        using clock = std::chrono::high_resolution_clock;
        auto cpuFrequency = 1.0 / clock::period::den; // 获取高精度计时器的频率
        auto start_time = clock::now(); // 获取开始时间

        for (int i = 0; i < m_maxFrames;)
        {
            auto time1 = clock::now(); // 获取当前时间
            m_frames[i].display(); // 显示当前帧
            auto time2 = clock::now(); // 获取显示完后的时间

            // 计算当前帧时间
            double frame_time = std::chrono::duration<double>(time2 - time1).count();

            if (i % static_cast<int>(m_fps) == 0)
            {
				printf("\r\033[0mfps: %3.2f,  frame: %d/%d", 1 / frame_time, i, m_maxFrames);
            }

            // 计算自开始以来的时间
            double time_since_start = std::chrono::duration<double>(time2 - start_time).count();
            i = static_cast<int>(m_fps * time_since_start);
        }
        printf("\n");
    }


    std::filesystem::path extract_audio(const std::filesystem::path& video_path)
    {
        std::string uuid = video_path.parent_path().string() + "\\" + CStringUtil::generate_uuid() + ".mp3";
        std::string command = ffmpeg + "ffmpeg -nostats -loglevel error -hide_banner -i " + video_path.string() +
            " -q:a 0 -map a " + uuid;
        int result = system(command.c_str());
        if (result != 0)
        {
            std::cerr << "Failed to execute command: " << command << std::endl;
            return "";
        }
        return uuid;
    }


    void play_sound()
    {
        if (m_audio_path.empty() || sound_playing) return;

        #ifdef _WIN32
        std::string openCommand = "open \"" + m_audio_path.string() + "\" type mpegvideo alias myMP3";
        if (mciSendStringA(openCommand.c_str(), NULL, 0, NULL) != 0)
        {
            std::cerr << "Failed to open the MP3 file." << std::endl;
            return;
        }

        if (mciSendStringA("play myMP3", NULL, 0, NULL) != 0)
        {
            std::cerr << "Failed to play the MP3 file." << std::endl;
            mciSendStringA("close myMP3", NULL, 0, NULL);
            return;
        }
        #else
        #endif
        sound_playing = true;
    }

public:
    void stop_sound()
    {
        if (!sound_playing) return;

        #ifdef _WIN32
        mciSendStringA("stop myMP3", NULL, 0, NULL);
        mciSendStringA("close myMP3", NULL, 0, NULL);
        #else
        #endif

        sound_playing = false;
    }
};

int enable_vt_mode()
{
#ifdef _WIN32
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hOut == INVALID_HANDLE_VALUE)
        return -1;
    DWORD dwMode = 0;
    if (!GetConsoleMode(hOut, &dwMode))
        return -2;
    dwMode |= 0x0004;
    if (!SetConsoleMode(hOut, dwMode))
        return -3;
    return 1;
#elif __linux__
	if (!isatty(STDOUT_FILENO)) {
		// 检查标准输出是否连接到终端
		std::cerr << "标准输出未连接到终端。" << std::endl;
		return -1;
	}

	// 尝试写入 ANSI 转义序列，测试终端是否支持
	std::cout << "\033[?25l" << std::flush; // 隐藏光标
	std::cout << "\033[?25h" << std::flush; // 恢复光标

	// 如果执行到这里没有问题，说明支持 ANSI
	return 1;
#endif
}


void reset_screen(int max_size, int font_size)
{
#ifdef _WIN32
    HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    if (hConsoleOutput == INVALID_HANDLE_VALUE)
    {
        std::cerr << "无法获取标准输出句柄" << std::endl;
        return;
    }

    CONSOLE_FONT_INFOEX fontInfo;
    fontInfo.cbSize = sizeof(CONSOLE_FONT_INFOEX);
    if (!GetCurrentConsoleFontEx(hConsoleOutput, FALSE, &fontInfo))
    {
        std::cerr << "无法获取当前控制台字体信息" << std::endl;
        return;
    }

    fontInfo.dwFontSize.X = 0;
    fontInfo.dwFontSize.Y = font_size;

    if (!SetCurrentConsoleFontEx(hConsoleOutput, FALSE, &fontInfo))
    {
        std::cerr << "无法设置新的控制台字体" << std::endl;
        return;
    }

    HWND consoleWindow = GetConsoleWindow();
    ShowWindow(consoleWindow, SW_MAXIMIZE);

    std::string s = "mode con cols=" + std::to_string(max_size) + " lines=" + std::to_string(max_size);
    system(s.data());
#endif
}