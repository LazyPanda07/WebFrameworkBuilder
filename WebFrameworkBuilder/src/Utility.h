#pragma once

#include <string>
#include <vector>

#include "INIParser.h"

namespace utility
{
	std::vector<std::string> parseData(const std::string& data);

	std::string getData(const std::string& typeOfData);

	void modifySlnFiles(const std::vector<std::string>& slnFiles, const INIParser& buildSettings);

	void modifyVcxprojFiles(const std::vector<std::string>& vcxprojFiles, const INIParser& buildSettings);
}
