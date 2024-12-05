#pragma once

#include "macro.h"

#include <filesystem>
#include <set>
#ifdef WIN
#include <windows.h>
#endif

#include "nlohmann/json.hpp"
#include "OpenXLSX.hpp"

namespace fs = std::filesystem;

class CFileUtil {
public:
	static bool read_file(const fs::path& path, std::string& data);

	static bool write_file(const fs::path& path, const std::string& data, const bool& append = true);

	static bool clear_file(const fs::path& path);

	static bool move_file(const fs::path& source_path, const fs::path& dest_path);

	static bool rename_file(const fs::path& old_name, const fs::path& new_name);

	static bool delete_file(const fs::path& file_path);

	static bool is_hidden(const fs::path& file);

#ifdef WIN
	static bool hide_file(const fs::path& file);

	static bool show_file(const fs::path& file);

	static bool modify_file_attribute(const fs::path& file, DWORD attribute, bool add);
#endif

	static void ensure_directory_exists(const fs::path& dir);

	static bool is_exists(const fs::path& f);

	static bool is_file(const fs::path& f);

	static bool is_dir(const fs::path& f);

	static fs::path get_exe_path();

	static fs::path get_project_dir();

	static fs::path get_exe_dir();

	static fs::path get_desktop_dir();

	static fs::path get_parent_dir(const fs::path& file);

	static fs::path relative_path(const fs::path& p1, const fs::path& p2);

	static bool is_file_in_use(const fs::path& filePath);

	static fs::path change_extension(const fs::path& file, const std::string& new_extension);

	static bool get_dir_files(const fs::path& dir, std::set<fs::path>& files, bool b_dir = false, bool recu = false);

	 static void write_excel(OpenXLSX::XLWorksheet& wks, int row, int col, const std::string& data);
	
	 static std::string read_excel(OpenXLSX::XLWorksheet& wks, int row, int col);

	static bool get_file_size(const fs::path& filePath, int& width, int& height);

	static bool jpg_to_png(const fs::path& jpgPath, const fs::path& pngPath);

	static bool png_to_jpg(const fs::path& pngPath, const fs::path& jpgPath);

	static fs::path compress_video(const fs::path& filePath, int maxSize, double xScale,
		double yScale, int& width, int& height);

	static fs::path compress_image(const fs::path& filePath, int maxSize, double xScale,
		double yScale, int& width, int& height);

	static bool get_file_json_info(const fs::path& file, nlohmann::json& j);

	static void ts_to_mp4(const fs::path& input_ts, const fs::path& output_mp4, std::string codec);

	static void mp4_to_ts(const fs::path& input_mp4, const fs::path& output_ts);
};

class CVideoInfo {
public:
	CVideoInfo(fs::path filepath);

	~CVideoInfo() {
	}

	long long get_duration() { return m_duration; }
	double get_fps() { return m_fps; }
	int get_frames() { return m_maxFrame; }
	int get_width() { return m_width; }
	int get_height() { return m_height; }
	std::vector<std::string> get_codecs() { return m_codecs; }

private:
	void init();

private:
	fs::path m_filepath;
	double m_fps;
	int m_maxFrame, m_width, m_height;
	long long m_duration;
	std::vector<std::string> m_codecs;
};
