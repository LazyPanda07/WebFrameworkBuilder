#pragma once

#include <string>
#include <vector>

namespace utility
{
	std::vector<std::string> parseData(const std::string& data);

	std::string getData(const std::string& typeOfData);

	void modifySlnFiles(const std::vector<std::string>& slnFiles);

	void modifyVcxprojFiles(const std::vector<std::string>& vcxprojFiles);
}
