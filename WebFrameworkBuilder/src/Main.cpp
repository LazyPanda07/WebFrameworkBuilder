#include <iostream>
#include <filesystem>

#include "Utility.h"

#pragma comment (lib, "INIParser.lib")

using namespace std;

void start(const vector<string>& targetVcxprojFiles, const vector<string>& targetSlnFiles, const utility::INIParser& buildSettings);

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

	cout << "Choose build variant" << endl
		<< "1. Modify .vcxproj files and .sln files" << endl
		<< "2. Modify .vcxproj files" << endl
		<< "3. Modify .sln files" << endl;

	while (true)
	{
		cin >> variant;

		if (variant == "1")
		{
			cin.ignore();

			targetVcxprojFiles = utility::parseData(utility::getData(".vcxproj files to modify"));
			targetSlnFiles = utility::parseData(utility::getData(".sln files to modify"));

			break;
		}
		else if (variant == "2")
		{
			cin.ignore();

			targetVcxprojFiles = utility::parseData(utility::getData(".vcxproj files to modify"));

			break;
		}
		else if (variant == "3")
		{
			cin.ignore();

			targetSlnFiles = utility::parseData(utility::getData(".sln files to modify"));

			break;
		}
		else
		{
			cout << "Try again" << endl;
		}
	}

	start(targetVcxprojFiles, targetSlnFiles, buildSettings);

	return 0;
}

void start(const vector<string>& targetVcxprojFiles, const vector<string>& targetSlnFiles, const utility::INIParser& buildSettings)
{
	if (targetVcxprojFiles.size() && targetSlnFiles.size())
	{
		utility::modifySlnFiles(targetSlnFiles, buildSettings);

		utility::modifyVcxprojFiles(targetVcxprojFiles, buildSettings);
	}
	else if (targetVcxprojFiles.size())
	{
		utility::modifyVcxprojFiles(targetVcxprojFiles, buildSettings);
	}
	else if (targetSlnFiles.size())
	{
		utility::modifySlnFiles(targetSlnFiles, buildSettings);
	}
}
