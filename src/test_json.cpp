#include <iostream>

#include "test.h"
#include "util/util.h"
#include "nlohmann/json.hpp"
#include "util/_string.h"
#include "OpenXLSX.hpp"
#include "util/_file.h"
#include "base64.h"

using namespace std;

namespace Test
{
	void test_json()
	{
		nlohmann::json j;

		j["name"] = "John";
		j["age"] = 30;
		j["is_student"] = false;

		j["hobbies"] =
		{ "reading", "cycling", "programming" };

		j["address"] =
		{
					{"city", "New York"},
					{"zipcode", "10001"}
		};

		j.erase("age");

		std::cout << j.dump(4) << std::endl; // ��� JSON �ַ��������� 4 �ո�



		std::string json_str = R"({
				"name": "С��",
				"age": 25,
				"is_student": true,
				"courses": ["Math", "Physics", "Chemistry"]
			})";


		json_str = base64_encode(json_str);
		json_str = base64_decode(json_str);
		json_str = CStringUtil::gbk_to_utf8(json_str);
		// ���ַ�������
		j = nlohmann::json::parse(json_str);

		std::cout << "Name: " << CStringUtil::utf8_to_gbk(j["name"]) << "\n";
		std::cout << "Age: " << j["age"] << "\n";
		std::cout << "Is Student: " << j["is_student"] << "\n";

		// ��������
		std::cout << "Courses: ";
		for (const auto& course : j["courses"])
		{
			std::cout << course << " ";
		}
		std::cout << std::endl;
	}

	void test_excel()
	{
		std::string str = "1a��@,��";
		str = CStringUtil::gbk_to_utf8(str);
		cout << (CStringUtil::is_utf8(str) ? "UTF8" : "GBK") << endl;

		OpenXLSX::XLDocument doc;
		doc.create("./test.xlsx", false);
		auto work = doc.workbook().worksheet("Sheet1");

		//doc.workbook().addWorksheet("1");
		//work = doc.workbook().worksheet("1");

		CFileUtil::write_excel(work, 1, 1, str);
		doc.save();
	}
}