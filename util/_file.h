#pragma once

#include <filesystem>
#include <set>

#include "OpenXLSX.hpp"

class _File
{
public:
	static bool read_file(const std::wstring& path, std::string& data);
	static bool write_file(const std::wstring& path, const std::string& data, const bool& append = true);
	static bool clear_file(const std::wstring& path);
	static bool move_file(const std::filesystem::path& source_path, const std::filesystem::path& dest_path);
	static bool rename_file(const std::filesystem::path& old_name, const std::filesystem::path& new_name);
	static bool delete_file(const std::filesystem::path& file_path);

	static bool is_hidden(const std::filesystem::path& file);

	static void ensure_directory_exists(std::filesystem::path dir);
	static std::filesystem::path get_exe_path();
	static std::filesystem::path get_exe_dir();
	static std::filesystem::path get_parent_dir(std::filesystem::path file);

	static std::set<std::filesystem::path> get_dir_files(std::filesystem::path dir, bool b_dir = false, bool recu = false);

	static void write_excel(OpenXLSX::XLWorksheet& wks, int row, int col, const std::string& data);
	static std::string read_excel(OpenXLSX::XLWorksheet& wks, int row, int col);

	static bool get_img_size(const std::wstring& filePath, int& width, int& height);
};