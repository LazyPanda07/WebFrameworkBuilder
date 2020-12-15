#include <iostream>
#include <filesystem>

#include "Utility.h"

#pragma comment (lib, "INIParser.lib")

using namespace std;

void start(const vector<string>& targetVcxprojFiles, const vector<string>& targetSlnFiles, const utility::INIParser& buildSettings, const unordered_map<string, string>& projGUID);

int main(int argc, char** argv)
{
	const filesystem::path buildFile(BUILD_FILE);

	if (!filesystem::exists(buildFile))
	{
		cout << "Can't find build_file.ini" << endl;

		return -1;
	}

	utility::INIParser buildSettings(buildFile);
	string variant;
	vector<string> targetVcxprojFiles;
	vector<string> targetSlnFiles;
	unordered_map<string, string> projGUID;	//vcxproj - GUID
	
	for (const auto& [i, j] : buildSettings.getSection("WebFramework"))
	{
		projGUID[j] = utility::getGUID();
	}

	cout << "Choose build variant" << endl
		<< "1. Modify .vcxproj files and .sln files" << endl
		<< "2. Modify .vcxproj files" << endl
		<< "3. Modify .sln files" << endl;

	while (true)
	{
		cin >> variant;

		if (variant == "1")
		{
			targetVcxprojFiles = utility::parseData(utility::getData(".vcxproj files to modify"));
			targetSlnFiles = utility::parseData(utility::getData(".sln files to modify"));

			break;
		}
		else if (variant == "2")
		{
			targetVcxprojFiles = utility::parseData(utility::getData(".vcxproj files to modify"));

			break;
		}
		else if (variant == "3")
		{
			targetSlnFiles = utility::parseData(utility::getData(".sln files to modify"));

			break;
		}
		else
		{
			cout << "Try again" << endl;
		}
	}

	start(targetVcxprojFiles, targetSlnFiles, buildSettings, projGUID);

	return 0;
}

void start(const vector<string>& targetVcxprojFiles, const vector<string>& targetSlnFiles, const utility::INIParser& buildSettings, const unordered_map<string, string>& projGUID)
{
	if (targetVcxprojFiles.size() && targetSlnFiles.size())
	{
		
	}
	else if (targetVcxprojFiles.size())
	{

	}
	else if (targetSlnFiles.size())
	{
		utility::modifySlnFiles(targetSlnFiles, buildSettings, projGUID);
	}
}
