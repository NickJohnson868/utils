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

static unsigned int ser_num = 1;
static string ser_name = "";

namespace Rename
{
	void reset_ser();
	bool contains(const vector<string>& vec, string value);
	bool execute_rename_file(const filesystem::path& file, string name, filesystem::path& new_file);
	void show(const set<filesystem::path>& files);



	string get_prefix(const filesystem::path& file)
	{
		string extension = file.extension().string();
		if (extension == ".png" || extension == ".jpg" || extension == ".jpeg" || extension == ".gif")
		{
			int w, h;
			_File::get_img_size(file.wstring(), w, h);
			return std::to_string(w) + "x" + std::to_string(h) + "_";
		}
		return "";
	}

	string get_suffix(const filesystem::path& file)
	{
		return "";
	}

	string uuid_file(const filesystem::path& file)
	{
		return get_prefix(file) + _String::generate_uuid() + get_suffix(file);
	}

	string datetime_file(const filesystem::path& file)
	{
		return get_prefix(file) + _Time::get_cur_datetime_ns("_", "_") + get_suffix(file);
	}

	string serial_number_file(const filesystem::path& file)
	{
		return get_prefix(file) + ser_name + to_string(ser_num++) + get_suffix(file);
	}

	void rename(const set<filesystem::path> &files, const vector<string>& exts, const int& switch_on)
	{
		auto method = uuid_file;

		switch (switch_on)
		{
		case 1:
			method = uuid_file;
			break;
		case 2:
			method = datetime_file;
			break;
		case 3:
			reset_ser();
			cout << "Please input a name: ";
			getline(cin, ser_name);
			method = serial_number_file;
			break;
		default:
			method = nullptr;
			break;
		}

		if (method == nullptr) return;

		for (auto& file : files)
		{
			string ext = file.extension().string().substr(1);
			_String::to_lower(ext);
			if (exts.empty() || contains(exts, ext))
			{
				filesystem::path new_name;
				if (execute_rename_file(file, method(file), new_name))
				{
					Util::color_print(P_R::_SUCCESS, "%s rename to %s success\n",
						file.filename().string().data(), new_name.filename().string().data());
				}
				else
				{
					Util::color_print(P_R::_ERROR, "%s rename faild\n", file.filename().string().data());
				}
			}
		}
	}









	void run()
	{
		while (true)
		{
			cout << "Please input work dir[or exit]: ";
			string work_dir;
			std::getline(std::cin, work_dir);

			if (work_dir == "exit") break;

			cout << "Whether to use recursion? [1]: ";
			string recursive;
			std::getline(std::cin, recursive);
			set<filesystem::path> files = _File::get_dir_files(work_dir, false,
				recursive == "1" || recursive == "y" || recursive == "Y");
			show(files);

			cout << "Please input extension[separated by ';', Null str is select all]: ";
			string ext_str;
			std::getline(std::cin, ext_str);
			vector<string> exts = _String::split_str(ext_str, ";");
			for (auto& ext : exts) _String::to_lower(ext);

			cout << "Please input rules: \n";
			cout << "1. UUID.ext\n";
			cout << "2. Datetime.ext\n";
			cout << "3. [A name] Serial number.ext\n";
			int switch_on = 0;
			cin >> switch_on;
			getchar();

			rename(files, exts, switch_on);
		}
	}

	void reset_ser()
	{
		ser_num = 1;
		ser_name = "";
	}

	bool contains(const vector<string>& vec, string value)
	{
		return find(vec.begin(), vec.end(), value) != vec.end();
	}

	bool execute_rename_file(const filesystem::path& file, string name, filesystem::path& new_file)
	{
		std::string extension = file.extension().string();
		new_file = file.parent_path() / (name + extension);
		return _File::rename_file(file, new_file);
	}

	void show(const set<filesystem::path>& files)
	{
		Table_DATA table;
		table.col_name.push_back("filename");
		table.col_name.push_back("extension");
		table.col_name.push_back("is hidden");

		for (const auto& file : files)
		{
			std::string filename = file.stem().string();
			std::string extension = file.extension().string();
			table.data[filename] = { extension, _File::is_hidden(file) ? "hide" : "show" };
		}
		Util::print_table(table);
	}
}