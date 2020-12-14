#include "Utility.h"

#include <iostream>
#include <array>

#include <Windows.h>

#include "Constants.h"

using namespace std;

string getGUID();

void addProjectsToSlnFile(const utility::INIParser& buildSetting, string& slnFile);

void eraseConfigurationPlatforms(string& slnFile);

namespace utility
{
	vector<string> parseData(const string& data)
	{
		vector<string> result
		(
			[&data]() -> size_t {
				size_t questionMarkCount = 0;

				for (const auto& i : data)
				{
					if (i == '?')
					{
						questionMarkCount++;
					}
				}

				return questionMarkCount + 1;
			}()
				);
		size_t startPath = 0;
		size_t endPath = 0;
		size_t currentIndex = 0;

		do
		{
			endPath = data.find('?', startPath);

			result[currentIndex++] = data.substr(startPath, endPath - startPath);

			startPath = endPath + 1;
		} while (endPath != string::npos);

		return result;
	}

	string getData(const string& typeOfData)
	{
		cout << "Enter " << typeOfData << " with '?' as delimiter" << endl;
		string data;

		cin.ignore();

		getline(cin, data);

		return data;
	}

	void modifySlnFiles(const vector<string>& slnFiles, const INIParser& buildSettings)
	{
		for (const auto& i : slnFiles)
		{
			fstream file(i, ios_base::in);
			string data;
			string tem;

			while (getline(file, tem))
			{
				data += tem + '\n';
			}

			eraseConfigurationPlatforms(data);

			addProjectsToSlnFile(buildSettings, data);

			file.close();

			file.open(i, ios_base::out);

			file << data;

			file.close();
		}
	}

	void modifyVcxprojFiles(const vector<string>& vcxprojFiles, const INIParser& buildSettings)
	{

	}
}

string getGUID()
{
	GUID guid;
	string guidString;

	if (CoCreateGuid(&guid) == S_OK)
	{
		wstring test;

		test.resize(39);

		test.resize(StringFromGUID2(guid, test.data(), test.size()));

		guidString.resize(test.size());

		for (size_t i = 0; i < test.size(); i++)
		{
			guidString[i] = static_cast<char>(test[i]);
		}

		guidString.pop_back();

		guidString = move(guidString);
	}
	else
	{
		throw runtime_error("Error create GUID");
	}

	return guidString;
}

void addProjectsToSlnFile(const utility::INIParser& buildSetting, string& slnFile)
{
	size_t lastEndProject = slnFile.rfind(sln::endProject) + sln::endProject.size() + 1;
	const unordered_multimap<string, string>& dependecies = buildSetting.getSection("WebFramework");
	size_t startProjectConfigurationPlatforms = slnFile.find('\n', slnFile.find(sln::projectConfigurationPlatforms)) + 1;
	string backSlashTString;
	string topLevelOfBackSlashTString;
	vector<string> guids;

	while (slnFile[startProjectConfigurationPlatforms] != '{')
	{
		backSlashTString += '\t';
		startProjectConfigurationPlatforms++;
	}

	topLevelOfBackSlashTString = string(backSlashTString.begin(), backSlashTString.end() - 1);

	auto addQuotes = [](const string& data) -> string
	{
		return '\"' + data + '\"';
	};

	auto addConfigurationPlatforms = [&slnFile, &backSlashTString](size_t startOffset, const string& guid) -> void
	{
		array<string, 3> platforms = { guid + '.' + "Debug|x64.", guid + '.' + "Release|x64.", guid + '.' + "ProductionRelease|x64." };

		for (const auto& i : platforms)
		{
			for (const auto& j : sln::buildConfigurations)
			{
				string addBuildConfiguration = backSlashTString + i;

				addBuildConfiguration += j;

				addBuildConfiguration += " = " + string(i.begin() + i.find('.') + 1, i.begin() + i.find('.', i.find('.') + 1)) + '\n';

				slnFile.insert(slnFile.begin() + startOffset, addBuildConfiguration.begin(), addBuildConfiguration.end());
			}
		}
	};

	for (const auto& [i, j] : dependecies)
	{
		string guid = getGUID();
		string addProject = "Project(" + sln::visualCPlusPlusProjectGUID + ") = " + addQuotes(j) + ", " + addQuotes(j + '\\' + j + extensions::projFile) + ", " + addQuotes(guid) +
			'\n' + sln::endProject + '\n';

		slnFile.insert(slnFile.begin() + lastEndProject, addProject.begin(), addProject.end());

		size_t startProjectConfigurationPlatforms = slnFile.find('\n', slnFile.find(sln::projectConfigurationPlatforms)) + 1;

		addConfigurationPlatforms(startProjectConfigurationPlatforms, guid);

		guids.push_back(move(guid));
	}

	string guid = getGUID();
	string solutionFolder = "Project(" + sln::solutionFolderProjectGUID + ") = " + addQuotes(sln::solutionFolderName) +
		", " + addQuotes(sln::solutionFolderName) + ", " + addQuotes(guid) +
		'\n' + sln::endProject + '\n';

	slnFile.insert(slnFile.begin() + lastEndProject, solutionFolder.begin(), solutionFolder.end());

	size_t preLastEndGlobalSection = slnFile.rfind(sln::endGlobalSection, slnFile.rfind(sln::endGlobalSection) + 1) + sln::endGlobalSection.size() + 1;
	string nestedProjects = topLevelOfBackSlashTString + sln::startGlobalSection + sln::nestedProjects + " = " + sln::preSolution + '\n';

	for (const auto& i : guids)
	{
		nestedProjects += backSlashTString + i + " = " + guid + '\n';
	}

	nestedProjects += topLevelOfBackSlashTString + sln::endGlobalSection + '\n';

	slnFile.insert(slnFile.begin() + preLastEndGlobalSection, nestedProjects.begin(), nestedProjects.end());
}

void eraseConfigurationPlatforms(string& slnFile)
{
	for (const auto& i : configurationsToDelete)
	{
		size_t findString = slnFile.find(i);
		size_t backSlashTCount = 0;

		while (slnFile[findString] != '\n')
		{
			findString--;
			backSlashTCount++;
		}

		findString++;

		slnFile.erase(findString, i.size() + backSlashTCount);
	}
}
