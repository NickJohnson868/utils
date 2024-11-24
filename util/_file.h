#pragma once

#include <filesystem>
#include "OpenXLSX.hpp"

class _File
{
public:
	static bool read_file(const std::wstring& path, std::string& data);
	static bool write_file(const std::wstring& path, const std::string& data, const bool& append = true);
	static bool clear_file(const std::wstring& path);

	static void ensure_directory_exists(std::filesystem::path dir);
	static std::filesystem::path get_exe_path();
	static std::filesystem::path get_exe_dir();
	static std::filesystem::path get_parent_dir(std::filesystem::path file);

	static void write_excel(OpenXLSX::XLWorksheet& wks, int row, int col, const std::string& data);
	static std::string read_excel(OpenXLSX::XLWorksheet& wks, int row, int col);
};