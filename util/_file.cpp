#include "_file.h"
#include "_string.h"
#include <windows.h>

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

bool _File::read_file(const std::wstring& path, std::string& data)
{
	std::ifstream file(path, std::ios::binary);
	if (!file)
	{
		return false;
	}
	std::string str((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
	data = _String::utf8_to_gbk(str);
	return true;
}

bool _File::write_file(const std::wstring& path, const std::string& data, const bool& append)
{
	auto flag = std::ios_base::binary;
	if (append) flag |= std::ios_base::app;
	std::ofstream file(path, flag);
	if (!file)
	{
		return false;
	}

	file << _String::gbk_to_utf8(data);
	return true;
}

bool _File::clear_file(const std::wstring& path)
{
	std::ofstream file(path, std::ios::trunc);
	return file.good();
}

// GBK¼ÇµÃ×ªUTF8
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
	wchar_t buffer[MAX_PATH];
	GetModuleFileNameW(NULL, buffer, MAX_PATH);
	std::filesystem::path path_ = std::wstring(buffer);
	return path_;
}

std::filesystem::path _File::get_exe_dir()
{
	return _File::get_exe_path().parent_path();
}

std::filesystem::path _File::get_parent_dir(std::filesystem::path file)
{
	return file.parent_path();
}
