#pragma once
#include "util/macro.h"

#ifdef WIN
#include <windows.h>
#include <mmsystem.h>
#define Y_SCALE 0.5
#if defined(MSVC)
#pragma comment(lib, "winmm.lib")
#endif
#elif defined(LINUX)
#include <unistd.h>
#define Y_SCALE 0.4
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
#include <thread>
#include <chrono>
#include "util/_proc.h"
#include "util/_time.h"

#define CHARACTER "#"
namespace fs = std::filesystem;

void reset_screen(int max_size = 192, int font_size = 8);

int enable_vt_mode();

struct Pixel {
	unsigned char R, G, B;
};

enum _Type {
	_IMAGE,
	_VIDEO,
	_GIF
};

class CFrame {
public:
	CFrame() {
	}

	virtual ~CFrame() {
	}

	void read_rgba(std::vector<uint8_t>& rgba_data, int width, int height) {
		std::ostringstream ss;
		ss.str().reserve(width * height);
		ss << "\033[H";

		const std::string color_prefix = "\033[38;2;";
		const std::string color_suffix = "m";
		const std::string reset_color = "\033[0m";

		for (int y = 0; y < height; ++y) {
			ss << "\033[2K"; // 清空行
			for (int x = 0; x < width; ++x) {
				int index = (y * width + x) * 4;
				uint8_t R = rgba_data[index];
				uint8_t G = rgba_data[index + 1];
				uint8_t B = rgba_data[index + 2];
				uint8_t A = rgba_data[index + 3];

#ifdef WIN
				RGB rgb = Util::rgba_to_rgb({ R, G, B, A }, { 12, 12, 12 }); // Windows终端背景色
#elif defined(LINUX)
				RGB rgb = Util::rgba_to_rgb({ R, G, B, A }, {  48, 10, 36 }); // ubuntu
#endif
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

	void display() {
		Util::fast_print(data.c_str(), data.size());
	}

private:
	std::string data;
	int m_width, m_height;
};

class CVideoPlayer {
public:
	CVideoPlayer() : m_frameWidth(0), m_frameHeight(0), m_fps(0) {
	}

	virtual ~CVideoPlayer() {
		if (CFileUtil::is_exists(m_audio_path))
		{
			CFileUtil::delete_file(m_audio_path);
		}
	}


	void display() {
		bool cursor_visible = Util::is_cursor_visible();
		Util::set_cursor_visible(false);
		play_frame();
		Util::set_cursor_visible(cursor_visible);
	}


	void read_video(const std::filesystem::path& videoPath, _Type type, int maxSize = 192) {
		CVideoInfo video(videoPath);
		m_frameWidth = video.get_width();
		m_frameHeight = video.get_height();
		std::cout << "压缩文件..." << std::endl;
		fs::path com_path;
		if (type == _VIDEO || type == _GIF) {
			m_fps = video.get_fps();
			m_maxFrames = video.get_frames();
			com_path = CFileUtil::compress_video(videoPath, maxSize, 1.0, Y_SCALE, m_frameWidth, m_frameHeight);
			if (type == _VIDEO) m_audio_path = extract_audio(videoPath);
		}
		else if (type == _IMAGE) {
			m_fps = 1;
			m_maxFrames = 1;
			com_path = CFileUtil::compress_image(videoPath, maxSize, 1.0, Y_SCALE, m_frameWidth, m_frameHeight);
		}
		else {
			return;
		}

		int frameCount = 0;
		std::string command = CStringUtil::format(
			"%sffmpeg -nostats -loglevel error -hide_banner -i \"%s\" -f image2pipe -pix_fmt rgba -vcodec rawvideo -vsync 0 -",
			FFMPEG_DIR, com_path.string().data());

		FILE* pipe = _popen(command.c_str(), OPEN_MODE);
		if (!pipe)
			throw std::runtime_error("ffmpeg command failed");

		std::cout << "读取视频帧数据..." << std::endl;
		bool cursor_visible = Util::is_cursor_visible();
		Util::set_cursor_visible(false);
		while (frameCount < m_maxFrames) {
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

private:
	std::vector<CFrame> m_frames;
	int m_frameWidth, m_frameHeight, m_maxFrames;
	double m_fps;
	std::filesystem::path m_audio_path;
	PID_TYPE ffplay_pid;

	void play_frame() {
		using clock = std::chrono::high_resolution_clock;
		auto cpuFrequency = 1.0 / clock::period::den; // 获取高精度计时器的频率
		auto start_time = clock::now(); // 获取开始时间

		create_sound_thread();
		if (ffplay_pid == -1) return;
		for (int i = 0; i < m_maxFrames;) {
			auto time1 = clock::now(); // 获取当前时间
			m_frames[i].display(); // 显示当前帧
			auto time2 = clock::now(); // 获取显示完后的时间

			// 计算当前帧时间
			double frame_time = std::chrono::duration<double>(time2 - time1).count();

			if (i % static_cast<int>(m_fps) == 0) {
				std::string info = CStringUtil::format("\033[2K\033[0mfps: %3.2f,  frame: %d/%d", 1 / frame_time, i, m_maxFrames);
				Util::fast_print(info.c_str(), info.size());
			}

			// 计算自开始以来的时间
			double time_since_start = std::chrono::duration<double>(time2 - start_time).count();
			i = static_cast<int>(m_fps * time_since_start);
		}
		exit_sound_thread();
		printf("\n");
	}


	std::filesystem::path extract_audio(const std::filesystem::path& video_path) {
		std::string uuid = video_path.parent_path().string() + FILE_SEP + CStringUtil::generate_uuid() + ".mp3";
		std::string command = CStringUtil::format(
			"%sffmpeg -nostats -loglevel error -hide_banner -i %s -q:a 0 -map a %s",
			FFMPEG_DIR, video_path.string().data(), uuid.data());
		int result = system(command.c_str());
		if (result != 0) {
			std::cerr << "Failed to execute command: " << command << std::endl;
			return "";
		}
		return uuid;
	}


	void create_sound_thread() {
		if (m_audio_path.empty()) return;

#ifdef WIN
		// 构建ffplay命令
		std::string command = CStringUtil::format("%sffplay.exe -hide_banner -loglevel quiet -nodisp %s",
			FFMPEG_DIR, m_audio_path.string().data());

		// 使用CreateProcess启动ffplay
		STARTUPINFOA si = { 0 };
		PROCESS_INFORMATION pi = { 0 };
		si.cb = sizeof(STARTUPINFOA);

		// 启动进程
		if (!CreateProcessA(
			nullptr,                           // 应用程序名称
			const_cast<char*>(command.c_str()), // 命令行参数
			nullptr,                           // 安全属性
			nullptr,                           // 线程安全属性
			FALSE,                             // 是否继承句柄
			0,                                 // 创建标志
			nullptr,                           // 环境变量
			nullptr,                           // 当前目录
			&si,                               // 启动信息
			&pi                                // 进程信息
		)) {
			std::cerr << "CreateProcess failed. Error code: " << GetLastError() << std::endl;
			return;
		}

		// 获取进程ID
		ffplay_pid = pi.dwProcessId;
		// 关闭句柄
		CloseHandle(pi.hProcess);
		CloseHandle(pi.hThread);

		// 等待进程完成（如果需要）
		// WaitForSingleObject(pi.hProcess, INFINITE);
#elif defined(LINUX)
		// 构建ffplay命令
		std::string command = CStringUtil::format("%sffplay -hide_banner -loglevel quiet -nodisp %s",
			FFMPEG_DIR, m_audio_path.string().data());
		pid_t pid = fork();

		if (pid < 0) {
			std::cerr << "fork failed. Error: " << strerror(errno) << std::endl;
			return;
		}
		else if (pid == 0) {
			char* args[10];
			int i = 0;

			char* token = strtok(const_cast<char*>(command.c_str()), " ");
			while (token != NULL && i < 9) {
				args[i++] = token;
				token = strtok(NULL, " ");
			}
			args[i] = NULL;
			// 执行命令
			execvp(args[0], args);
			exit(EXIT_FAILURE);
		}
		else {
			// 父进程，记录子进程的 PID
			ffplay_pid = pid;
			// 可以选择等待子进程完成
			// waitpid(pid, NULL, 0);
		}
#endif
		CTimeUtil::sleep(100);
	}

	void exit_sound_thread() {
		if (ffplay_pid == -1) return;
		CProcUtil::close_process_by_id(ffplay_pid);
		if (CFileUtil::is_exists(m_audio_path)) {
			while (!CFileUtil::delete_file(m_audio_path)) {
				CTimeUtil::sleep(100);
			}
		}
	}
};

int enable_vt_mode() {
#ifdef WIN
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
#elif defined(LINUX)
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


void reset_screen(int max_size, int font_size) {
#ifdef WIN
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