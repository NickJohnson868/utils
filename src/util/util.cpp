#include "util.h"

#include <cstdarg>
#include <random>
#include <numeric>
#include <fstream>
#include <sstream>

#ifdef WIN
#include <windows.h>
#elif defined(LINUX)
#include <unistd.h>
#endif

bool cursor_visible = true;

void Util::set_cursor_visible(bool visible) {
	visible ? printf("\033[?25h") : printf("\033[?25l");
	fflush(stdout);
	cursor_visible = visible;
}

bool Util::is_cursor_visible() {
	return cursor_visible;
}

void Util::print_progress_bar(long long now, long long total, const char* title) {
	constexpr int bar_width = 50;

	if (now == -1) {
		// �����ǰ��
		printf("\033[2K");
		fflush(stdout);
		return;
	}
	if (total <= 0)
		return;

	const double progress = static_cast<double>(now) / static_cast<double>(total);
	const int pos = static_cast<int>(bar_width * progress);

	/* ��ӡ������ */
	printf(" %s[", title);
	for (int i = 0; i < bar_width; ++i) {
		if (i < pos)
			printf("=");
		else if (i == pos)
			printf(">");
		else
			printf(" ");
	}
	printf("] %.2f%%\r", progress * 100.0);
	fflush(stdout);
}


void Util::color_print(PrintColor color, const char* Format, ...) {
	const char* colorCode = "";

	switch (color) {
	case P_ERROR:
		colorCode = "\033[31m"; /* ��ɫ */
		break;
	case P_WARNING:
		colorCode = "\033[33m"; /* ��ɫ */
		break;
	case P_INFO:
		colorCode = "\033[34m"; /* ��ɫ */
		break;
	case P_SUCCESS:
		colorCode = "\033[32m"; /* ��ɫ */
		break;
	case P_NORMAL:
		colorCode = "\033[37m";
		break;
	}

	va_list args;
	va_start(args, Format);
	printf("%s", colorCode); /* ��ӡ��ɫ */
	vprintf(Format, args); /* ��ӡ��ʽ������ */
	printf("\033[0m"); /* ������ɫ */
	va_end(args);
}


void Util::color_print(RGB color, const char* Format, ...) {
	unsigned char r = color.R;
	unsigned char g = color.G;
	unsigned char b = color.B;

	/* ʹ�� va_list ������ɱ���� */
	va_list args;
	va_start(args, Format);

	/* ����ǰ��ɫΪ����� RGB */
	printf("\033[38;2;%d;%d;%dm", r, g, b);
	vprintf(Format, args); /* ��ӡ��ʽ������ */
	printf("\033[0m"); /* ������ɫ */
	va_end(args);
}
#ifdef WIN
DWORD written;
HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
#endif
void Util::fast_print(const char* data, size_t size)
{
#ifdef WIN
	WriteConsoleA(hConsole, data, size, &written, NULL);
#elif defined(LINUX)
	ssize_t written = write(STDOUT_FILENO, data, size);
#endif
}

void print_line(const char chr, const int len) {
	for (int i = 0; i < len; i++)
		putchar(chr);
	printf("\n");
}


void Util::print_table(const Table_Data& table) {
	if (table.empty()) {
		return;
	}

	/* ����ÿ�е������ */
	std::vector<int> col_width(table[0].size(), 0); /* ����������ʼ����� */

	for (const auto& i : table) {
		for (size_t j = 0; j < i.size(); ++j) {
			if (const size_t len = i[j].length(); len > col_width[j]) {
				col_width[j] = static_cast<int>(len); /* �����е������ */
			}
		}
	}

	/* ��ӡ��ͷ */
	print_line(
		'-', static_cast<int>(std::accumulate(col_width.begin(), col_width.end(), 0) + 3 * (col_width.size()) + 1));
	for (size_t i = 0; i < table[0].size(); ++i) {
		printf("| %-*s ", col_width[i], table[0][i].data());
	}
	printf("|\n");
	print_line(
		'-', static_cast<int>(std::accumulate(col_width.begin(), col_width.end(), 0) + 3 * (col_width.size()) + 1));

	/* ��ӡ������ */
	for (size_t i = 1; i < table.size(); ++i) {
		/* ��1��ʼ����Ϊ��0���Ǳ�ͷ */
		for (size_t j = 0; j < table[i].size(); ++j) {
			Util::color_print(P_SUCCESS, "| %-*s ", col_width[j], table[i][j].data());
		}
		Util::color_print(P_SUCCESS, "|\n");
		print_line(
			'-', static_cast<int>(std::accumulate(col_width.begin(), col_width.end(), 0) + 3 * (col_width.size()) + 1));
	}
}


bool Util::get_cmd_output(const std::string& command, std::string& output) {
	FILE* pipe = _popen(command.c_str(), OPEN_MODE);

	if (!pipe) {
		Util::color_print(PrintColor::P_ERROR, "error opening pipe for ffprobe\n");
		return false;
	}

	std::stringstream buffer;
	char line[256]{};

	while (fgets(line, sizeof(line), pipe) != nullptr) {
		buffer << line;
	}

	output = buffer.str();
	_pclose(pipe);
	return true;
}

RGB Util::rgba_to_rgb(const RGBA& rgba, const RGB& bk) {
	RGB rgb;
	const float alpha = static_cast<float>(rgba.A) / 255.0f;
	rgb.R = static_cast<unsigned char>(alpha * static_cast<float>(rgba.R) + (1 - alpha) * static_cast<float>(bk.R));
	rgb.G = static_cast<unsigned char>(alpha * static_cast<float>(rgba.G) + (1 - alpha) * static_cast<float>(bk.G));
	rgb.B = static_cast<unsigned char>(alpha * static_cast<float>(rgba.B) + (1 - alpha) * static_cast<float>(bk.B));
	return rgb;
}