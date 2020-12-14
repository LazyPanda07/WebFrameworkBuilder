#include <iostream>
#include <filesystem>

#include "Utility.h"

#pragma comment (lib, "INIParser.lib")

using namespace std;

int main(int argc, char** argv)
{
	const filesystem::path buildFile("build_file.ini");

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


	return 0;
}
