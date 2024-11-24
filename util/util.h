#pragma once

#include <string>

class Util
{
public:
	static void set_cursor_info(bool visible);
	static std::string generate_uuid(bool b_upper = false, bool b_delimiter = true);
	static void print_progress_bar(int row, int total);
};