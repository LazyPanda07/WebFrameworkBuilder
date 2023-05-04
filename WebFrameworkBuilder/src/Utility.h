#pragma once

#include <string>
#include <vector>

#include "JSONParser.h"

namespace utility
{
	std::vector<std::string> parseData(const std::string& data);

	std::string getData(const std::string& typeOfData);

	void modifySlnFiles(const std::vector<std::string>& slnFiles, const json::JSONParser& buildSettings);

	void modifyVcxprojFiles(const std::vector<std::string>& vcxprojFiles, const json::JSONParser& buildSettings);

	std::string getGUID();
}
