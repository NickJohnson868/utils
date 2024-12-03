#include <string>
#include <iostream>
#include <algorithm>
#include <set>
#include <filesystem>

#include "rename.h"
#include "util/_string.h"
#include "util/_time.h"
#include "util/_file.h"
#include "util/util.h"

using namespace std;

static unsigned int	ser_num = 1;
static string work_dir = "", prefix = "", suffix = "";

namespace Rename
{
	void reset_ser();

	bool contains(const vector<string>& vec, string value);

	bool execute_rename_file(const filesystem::path& file, string name, filesystem::path& new_file);

	void show(const set<filesystem::path>& files, const string& work_dir);

	bool is_image(const string& ext);

	bool is_video(const string& ext);

	void scan_dir(const string& work_dir, std::set<fs::path>& files)
	{
		files.clear();
		CFileUtil::get_dir_files(work_dir, files, false, false);
		show(files, work_dir);
	}

	string get_file_reso(const filesystem::path& file)
	{
		string	extension = file.extension().string();
		int	w = 0, h = 0;
		if (is_image(extension)) {
			CFileUtil::get_file_size(file, w, h);
		}
		else {
			return "";
		}

		return std::to_string(w) + "x" + std::to_string(h);
	}


	string uuid_file(const filesystem::path& file)
	{
		return((is_image(file.extension().string()) ? get_file_reso(file) + "_" : "")
			+ prefix + CStringUtil::generate_uuid() + suffix);
	}


	string datetime_file(const filesystem::path& file)
	{
		return((is_image(file.extension().string()) ? get_file_reso(file) + "_" : "")
			+ prefix + CTimeUtil::get_cur_datetime_ns("_", "_") + suffix);
	}


	string serial_number_file(const filesystem::path& file)
	{
		return prefix + to_string(ser_num++) + suffix;
	}


	void rename(set<filesystem::path>& files)
	{
		vector<string> exts;
		exts.push_back("-1");
	start:
		cout << "\n";
		Util::color_print(P_INFO, "step 3-1-2: \n");
		cout << "now exts: ";
		if (exts.empty()) {
			cout << "all";
		}
		else {
			for (auto& e : exts) cout << e << " ";
		}
		cout << "\n";
		cout << "0. back\n";
		cout << "1. input ext\n";
		cout << "2. uuid.ext\n";
		cout << "3. datetime.ext\n";
		cout << "4. [prefix] serial number [suffix].ext\n";
		cout << "5. rescan dir\n";
		cout << "please: ";
		string num;
		std::getline(std::cin, num);
		reset_ser();

		auto method = uuid_file;

		if (num == "0") {
			return;
		}
		else if (num == "1") {
			cout << "\n";
			cout << "please input extension[png;jpg, Null str is select all]: ";
			string ext_str;
			std::getline(std::cin, ext_str);
			exts.clear();
			exts = CStringUtil::split_str(ext_str, ";");
			for (auto& ext : exts) CStringUtil::to_lower(ext);
			goto start;
		}
		else if (num == "2") {
			method = uuid_file;
		}
		else if (num == "3") {
			method = datetime_file;
		}
		else if (num == "4") {
			cout << "\n";
			cout << "please input prefix: ";
			getline(cin, prefix);
			cout << "please input suffix: ";
			getline(cin, suffix);
			method = serial_number_file;
		}
		else if (num == "5") {
			scan_dir(work_dir, files);
			goto start;
		}
		else {
			goto start;
		}

		if (exts.size() && exts[0] == "-1") goto start;
		for (auto& file : files)
		{
			string ext = file.extension().string().substr(1);
			CStringUtil::to_lower(ext);
			if (exts.empty() || contains(exts, ext))
			{
				filesystem::path new_name;
				if (execute_rename_file(file, method(file), new_name))
				{
					Util::color_print(P_SUCCESS, "rename %s to %s success\n",
						file.filename().string().data(), new_name.filename().string().data());
				}
				else {
					Util::color_print(P_ERROR, "%s rename faild\n", file.filename().string().data());
				}
			}
		}
		scan_dir(work_dir, files);
		goto start;
	}


	void convert(set<filesystem::path>& files)
	{
	start:
		cout << "\n";
		Util::color_print(P_INFO, "step 3-2-1: \n");
		cout << "0. back\n";
		cout << "1. ts to h264\n";
		cout << "2. ts to h265\n";
		cout << "3. mp4 to ts(h264)\n";
		cout << "4. rescan dir\n";
		cout << "5. delete file\n";
		cout << "please: ";
		string num;
		std::getline(std::cin, num);

		if (num == "0") {
			return;
		}
		else if (num == "1" || num == "2") {
			string codec = "h264";
			if (num == "2")
				codec = "h265";
			for (auto& file : files)
			{
				if (file.extension().string() == ".ts")
				{
					filesystem::path new_name = CFileUtil::change_extension(file, ".mp4");
					CFileUtil::ts_to_mp4(file.string(), new_name.string(), codec);
				}
			}
		}
		else if (num == "3") {
			for (auto& file : files)
			{
				if (file.extension().string() == ".mp4")
				{
					filesystem::path new_name = CFileUtil::change_extension(file, ".ts");
					CFileUtil::mp4_to_ts(file.string(), new_name.string());
				}
			}
		}
		else if (num == "4") {
			scan_dir(work_dir, files);
			goto start;
		}
		else if (num == "5") {
			cout << "\n";
			cout << "please input: ";
			string wildcard;
			std::getline(std::cin, wildcard);
			for (auto& file : files)
			{
				if (CStringUtil::match_wildcard(file.filename().string(), wildcard))
				{
					CFileUtil::delete_file(file);
					Util::color_print(P_SUCCESS, "delete %s success\n",
						file.filename().string().data());
				}
			}
		}
		scan_dir(work_dir, files);
		goto start;
	}

	void run()
	{
		while (true)
		{
		input_dir:
			cout << "\n";
			Util::color_print(P_INFO, "step 1: ");
			cout << "please input work dir: ";
			work_dir = "";
			std::getline(std::cin, work_dir);
			if (!CFileUtil::is_exists(work_dir))
			{
				cout << "dir is not exists" << endl;
				goto input_dir;
			}
			std::set<fs::path> files;
			scan_dir(work_dir, files);

		rename_or_convert:
			{
				cout << "\n";
				Util::color_print(P_INFO, "step 2: \n");
				cout << "0. back\n";
				cout << "1. rename file\n";
				cout << "2. convert video\n";
				cout << "please: ";
				string r_or_v;
				std::getline(std::cin, r_or_v);
				if (r_or_v == "0") {
					goto input_dir;
				}
				else if (r_or_v == "1") {
					goto rename_file;
				}
				else if (r_or_v == "2") {
					goto convert_video;
				}
				else {
					goto rename_or_convert;
				}
			}

		rename_file:
			{
				rename(files);
				goto rename_or_convert;
			}

		convert_video:
			{
				convert(files);
				goto rename_or_convert;
			}
		}
	}

	void reset_ser()
	{
		ser_num = 1;
		prefix = "";
		suffix = "";
	}


	bool contains(const vector<string>& vec, string value)
	{
		return find(vec.begin(), vec.end(), value) != vec.end();
	}


	bool execute_rename_file(const filesystem::path& file, string name, filesystem::path& new_file)
	{
		std::string extension = file.extension().string();
		new_file = file.parent_path() / (name + extension);
		return CFileUtil::rename_file(file, new_file);
	}


	void show(const std::set<filesystem::path>& files, const std::string& work_dir)
	{
		cout << "Reading directory information" << endl;
		bool cursor_visible = Util::is_cursor_visible();
		Util::set_cursor_visible(false);
		Table_Data table;
		table.push_back({ "order", "filename", "extension", "in use" });

		int i = 1;
		Util::print_progress_bar(0, files.size());
		for (const auto& file : files)
		{
			filesystem::path	rel_path = CFileUtil::relative_path(file, work_dir);
			std::string		filename = rel_path.stem().string();
			std::string		extension = rel_path.extension().string();

			/* 提取相对路径中的文件夹结构 */
			std::string folder = rel_path.parent_path().string();
			if (!folder.empty())
			{
				filename = folder + FILE_SEP + filename;
			}

			table.push_back({ std::to_string(i), filename, extension,
					   CFileUtil::is_file_in_use(file) ? "true" : "false" });
			Util::print_progress_bar(i, files.size());
			i++;
		}
		Util::print_progress_bar(i, files.size());
		Util::print_progress_bar(-1, -1);

		Util::set_cursor_visible(cursor_visible);
		Util::print_table(table);
	}


	bool is_image(const string& extension)
	{
		return(
			extension == ".png" || extension == ".jpg" ||
			extension == ".jpeg" || extension == ".gif"
			);
	}


	bool is_video(const string& extension)
	{
		return(
			extension == ".mp4" || extension == ".avi" ||
			extension == ".mkv" || extension == ".flv"
			);
	}
}