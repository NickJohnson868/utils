#include "_file.h"
#include "util.h"

#include <windows.h>
#include <gdiplus.h>

#ifdef _DEBUG
#ifdef _WIN64
#pragma comment(lib, "x64/OpenXLSXd.lib")
#elif defined(_WIN32)
#pragma comment(lib, "x86/OpenXLSXd.lib")
#endif
#else
#ifdef _WIN64
#pragma comment(lib, "x64/OpenXLSX.lib")
#elif defined(_WIN32)
#pragma comment(lib, "x86/OpenXLSX.lib")
#endif
#endif

#pragma comment(lib, "gdiplus.lib")

// 内存中保持GBK编码
bool _File::read_file(const std::wstring& path, std::string& data)
{
	std::ifstream file(path, std::ios::binary);
	if (!file)
	{
		return false;
	}
	std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	data = str;
	return true;
}

// 推荐写入UTF8
bool _File::write_file(const std::wstring& path, const std::string& data, const bool& append)
{
	auto flag = std::ios_base::binary;
	if (append) flag |= std::ios_base::app;
	std::ofstream file(path, flag);
	if (!file)
		return false;

	file << data;
	return true;
}

bool _File::clear_file(const std::wstring& path)
{
	std::ofstream file(path, std::ios::trunc);
	return file.good();
}

bool _File::move_file(const std::filesystem::path& source_path, const std::filesystem::path& dest_path)
{
	return rename_file(source_path, dest_path);
}

bool _File::rename_file(const std::filesystem::path& old_name, const std::filesystem::path& new_name)
{
	try
	{
		if (std::filesystem::exists(new_name))
		{
			Util::color_print(P_R::_ERROR, "new file is exists\n");
			return false;
		}
		std::filesystem::rename(old_name, new_name);
		return true;
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		return false;
	}
}

bool _File::delete_file(const std::filesystem::path& file_path)
{
	try
	{
		std::filesystem::remove(file_path);
		return true;
	}
	catch (const std::filesystem::filesystem_error& e)
	{
		return false;
	}
}

bool _File::is_hidden(const std::filesystem::path& file)
{
	DWORD fileAttributes = GetFileAttributes(file.c_str());
	return (fileAttributes != INVALID_FILE_ATTRIBUTES) && (fileAttributes & FILE_ATTRIBUTE_HIDDEN);
}

// GBK记得转UTF8
void _File::write_excel(OpenXLSX::XLWorksheet& wks, int row, int col, const std::string& data)
{
	wks.cell(OpenXLSX::XLCellReference(row, col)).value() = data;
}

std::string _File::read_excel(OpenXLSX::XLWorksheet& wks, int row, int col)
{
	return wks.cell(OpenXLSX::XLCellReference(row, col)).value().get<std::string>();
}

void _File::ensure_directory_exists(std::filesystem::path dir)
{
	if (!std::filesystem::exists(dir))
		std::filesystem::create_directories(dir);
}

std::filesystem::path _File::get_exe_path()
{
	return std::filesystem::current_path();
}

std::filesystem::path _File::get_exe_dir()
{
	return _File::get_exe_path().parent_path();
}

std::filesystem::path _File::get_parent_dir(std::filesystem::path file)
{
	return file.parent_path();
}

std::set<std::filesystem::path> _File::get_dir_files(std::filesystem::path dir, bool b_dir, bool b_recu)
{
	std::set<std::filesystem::path> files;

	auto process_entry = [&](const std::filesystem::directory_entry& entry)
		{
			bool is_file = std::filesystem::is_regular_file(entry);
			bool is_directory = std::filesystem::is_directory(entry);

			if ((b_dir && is_directory) || (!b_dir && is_file)) {
				files.insert(entry.path());
			}
		};

	try
	{
		if (!std::filesystem::exists(dir) || !std::filesystem::is_directory(dir)) return files;
		if (b_recu) for (const auto& entry : std::filesystem::recursive_directory_iterator(dir)) process_entry(entry);
		else for (const auto& entry : std::filesystem::directory_iterator(dir)) process_entry(entry);
	}
	catch (const std::exception& e) {
		std::cerr << "Error: " << e.what() << std::endl;
	}

	return files;
}

bool _File::get_img_size(const std::wstring& filePath, int& width, int& height)
{
	Gdiplus::GdiplusStartupInput gdiplusStartupInput;
	ULONG_PTR gdiplusToken;
	Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

	bool res = false;
	Gdiplus::Bitmap* image = new Gdiplus::Bitmap(filePath.c_str(), FALSE);
	if (image->GetLastStatus() == Gdiplus::Ok)
	{
		width = image->GetWidth();
		height = image->GetHeight();
		res = true;
	}
	delete image;
	Gdiplus::GdiplusShutdown(gdiplusToken);
	return res;
}