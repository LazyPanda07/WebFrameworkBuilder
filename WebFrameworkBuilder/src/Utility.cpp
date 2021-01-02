#include "Utility.h"

#include <iostream>
#include <filesystem>

#include <Windows.h>

#include "Constants.h"

using namespace std;

void addProjectsToSlnFile(const utility::INIParser& buildSetting, string& slnFile);

void eraseConfigurationPlatforms(string& slnFile);

void addAdditionalIncludeDirectories(const utility::INIParser& buildSettings, string& vcxprojFile);

void addAdditionalDependencies(const utility::INIParser& buildSettings, string& vcxprojFile);

void addAdditionalLibraryDirectories(string& vcxprojFile);

void addProjectReference(const utility::INIParser& buildSettings, string& vcxprojFile);

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

		getline(cin, data);

		if (!filesystem::exists(data))
		{
			throw runtime_error("File " + data + " doesn't exist");
		}

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
		for (const auto& i : vcxprojFiles)
		{
			fstream file(i, ios_base::in);
			string data;
			string tem;

			while (getline(file, tem))
			{
				data += tem + '\n';
			}

			addAdditionalIncludeDirectories(buildSettings, data);

			addAdditionalDependencies(buildSettings, data);

			addAdditionalLibraryDirectories(data);

			addProjectReference(buildSettings, data);

			file.close();

			file.open(i, ios_base::out);

			file << data;

			file.close();
		}
	}

	string utility::getGUID()
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
}

void addProjectsToSlnFile(const utility::INIParser& buildSetting, string& slnFile)
{
	size_t lastEndProject = slnFile.rfind(sln::endProject) + sln::endProject.size() + 1;
	const unordered_multimap<string, string>& dependencies = buildSetting.getSection(webFrameworkSettings);
	size_t startProjectConfigurationPlatforms = slnFile.find('\n', slnFile.find(sln::projectConfigurationPlatforms)) + 1;
	string backSlashTString;
	string topLevelOfBackSlashTString;

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

	for (const auto& i : dependencies)
	{
		string addProject = "Project(" + sln::visualCPlusPlusProjectGUID + ") = " + addQuotes(i.first) + ", " + addQuotes(webFrameworkFolder + '\\' + i.first + '\\' + i.first + extensions::projFile) + ", " + addQuotes(i.second) +
			'\n' + sln::endProject + '\n';

		slnFile.insert(slnFile.begin() + lastEndProject, addProject.begin(), addProject.end());

		size_t startProjectConfigurationPlatforms = slnFile.find('\n', slnFile.find(sln::projectConfigurationPlatforms)) + 1;

		addConfigurationPlatforms(startProjectConfigurationPlatforms, i.second);
	}

	string guid = utility::getGUID();
	string solutionFolder = "Project(" + sln::solutionFolderProjectGUID + ") = " + addQuotes(sln::solutionFolderName) +
		", " + addQuotes(sln::solutionFolderName) + ", " + addQuotes(guid) +
		'\n' + sln::endProject + '\n';

	slnFile.insert(slnFile.begin() + lastEndProject, solutionFolder.begin(), solutionFolder.end());

	size_t preLastEndGlobalSection = slnFile.rfind(sln::endGlobalSection, slnFile.rfind(sln::endGlobalSection) + 1) + sln::endGlobalSection.size() + 1;
	string nestedProjects = topLevelOfBackSlashTString + sln::startGlobalSection + sln::nestedProjects + " = " + sln::preSolution + '\n';

	for (const auto& i : dependencies)
	{
		nestedProjects += backSlashTString + i.second + " = " + guid + '\n';
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

void addAdditionalIncludeDirectories(const utility::INIParser& buildSettings, string& vcxprojFile)
{
	size_t startClCompile = vcxprojFile.find(vcxproj::clCompileTag) + vcxproj::clCompileTag.size() + 1;
	size_t stopOffset = startClCompile;
	string spacesString;
	const unordered_multimap<string, string>& dependencies = buildSettings.getSection(webFrameworkSettings);

	while (vcxprojFile[startClCompile] != '<')
	{
		spacesString += ' ';
		startClCompile++;
	}

	string addAdditionalIncludeDirectories;

	for (const auto& i : dependencies)
	{
		addAdditionalIncludeDirectories += vsMacros::solutionDir + webFrameworkFolder + "\\" + i.first + "\\src;";
	}

	if (vcxprojFile.find(vcxproj::startAdditionalIncludeDirectoriesTag) == string::npos)
	{
		addAdditionalIncludeDirectories.insert(addAdditionalIncludeDirectories.begin(), vcxproj::startAdditionalIncludeDirectoriesTag.begin(), vcxproj::startAdditionalIncludeDirectoriesTag.end());

		addAdditionalIncludeDirectories.insert(addAdditionalIncludeDirectories.begin(), spacesString.begin(), spacesString.end());

		startClCompile = vcxprojFile.find(vcxproj::clCompileTag) + vcxproj::clCompileTag.size() + 1;

		addAdditionalIncludeDirectories += vcxproj::endAdditionalIncludeDirectoriesTag + '\n';

		while (startClCompile >= stopOffset)
		{
			vcxprojFile.insert(vcxprojFile.begin() + startClCompile, addAdditionalIncludeDirectories.begin(), addAdditionalIncludeDirectories.end());

			startClCompile = vcxprojFile.find(vcxproj::clCompileTag, startClCompile) + vcxproj::clCompileTag.size() + 1;
		}
	}
	else
	{
		size_t next = vcxprojFile.find(vcxproj::endAdditionalIncludeDirectoriesTag);

		while (next != string::npos)
		{
			if (vcxprojFile[next - 1] == ';')
			{
				vcxprojFile.insert(vcxprojFile.begin() + next, addAdditionalIncludeDirectories.begin(), addAdditionalIncludeDirectories.end());
			}
			else
			{
				vcxprojFile.insert(vcxprojFile.insert(vcxprojFile.begin() + next, ';') + 1, addAdditionalIncludeDirectories.begin(), addAdditionalIncludeDirectories.end());
			}

			next = vcxprojFile.find(vcxproj::endAdditionalIncludeDirectoriesTag, vcxprojFile.find(vcxproj::endAdditionalIncludeDirectoriesTag, next) + 1);
		}
	}
}

void addAdditionalDependencies(const utility::INIParser& buildSettings, string& vcxprojFile)
{
	const string& libraryName = buildSettings.getKeyValue(webFrameworkLink, "linkLibrary");
	size_t startLink = vcxprojFile.find(vcxproj::startLinkTag) + vcxproj::startLinkTag.size() + 1;
	string spacesString;

	while (vcxprojFile[startLink] != '<')
	{
		spacesString += ' ';
		startLink++;
	}

	const string additionalDependenciesString = spacesString + vcxproj::startAdditionalDependenciesTag + libraryName + ';' + vcxproj::additionalDependenciesMacro + vcxproj::endAdditionalDependenciesTag + '\n';
	startLink = vcxprojFile.find(vcxproj::startLinkTag) + vcxproj::startLinkTag.size() + 1;

	while (true)
	{
		size_t checkAdditionalDependencies = vcxprojFile.find(vcxproj::endAdditionalDependenciesTag, startLink);

		if (checkAdditionalDependencies == string::npos)
		{
			vcxprojFile.insert(vcxprojFile.begin() + startLink, additionalDependenciesString.begin(), additionalDependenciesString.end());

			startLink = vcxprojFile.find(vcxproj::startLinkTag, startLink + additionalDependenciesString.size());
		}
		else
		{
			string appendAdditionalDependencies = ';' + libraryName;

			vcxprojFile.insert(vcxprojFile.begin() + checkAdditionalDependencies, appendAdditionalDependencies.begin(), appendAdditionalDependencies.end());

			startLink = vcxprojFile.find(vcxproj::startLinkTag, startLink + appendAdditionalDependencies.size());
		}

		if (startLink == string::npos)
		{
			break;
		}

		startLink += vcxproj::startLinkTag.size() + 1;
	}
}

void addAdditionalLibraryDirectories(string& vcxprojFile)
{
	const string libPaths = R"($(SolutionDir)bin\$(Configuration)-$(Platform)\)" + webFrameworkName + ';' + "$(SolutionDir)" + webFrameworkFolder + '\\' + webFrameworkName + "\\libs";
	size_t startLink = vcxprojFile.find(vcxproj::startLinkTag) + vcxproj::startLinkTag.size() + 1;
	string spacesString;

	while (vcxprojFile[startLink] != '<')
	{
		spacesString += ' ';
		startLink++;
	}

	const string additionalLibraryDirectoriesString = spacesString + vcxproj::startAdditionalLibraryDirectoriesTag + libPaths + vcxproj::endAdditionalLibraryDirectoriesTag + '\n';
	startLink = vcxprojFile.find(vcxproj::startLinkTag) + vcxproj::startLinkTag.size() + 1;

	while (true)
	{
		size_t checkAdditionalLibraryDirectories = vcxprojFile.find(vcxproj::endAdditionalLibraryDirectoriesTag, startLink);

		if (checkAdditionalLibraryDirectories == string::npos)
		{
			vcxprojFile.insert(vcxprojFile.begin() + startLink, additionalLibraryDirectoriesString.begin(), additionalLibraryDirectoriesString.end());

			startLink = vcxprojFile.find(vcxproj::startLinkTag, startLink + additionalLibraryDirectoriesString.size());
		}
		else
		{
			string appendAdditionalLibraryDirectories = ';' + libPaths;

			vcxprojFile.insert(vcxprojFile.begin() + checkAdditionalLibraryDirectories, appendAdditionalLibraryDirectories.begin(), appendAdditionalLibraryDirectories.end());

			startLink = vcxprojFile.find(vcxproj::startLinkTag, startLink + appendAdditionalLibraryDirectories.size());
		}

		if (startLink == string::npos)
		{
			break;
		}

		startLink += vcxproj::startLinkTag.size() + 1;
	}
}

void addProjectReference(const utility::INIParser& buildSettings, string& vcxprojFile)
{
	string webFrameworkGUID = buildSettings.getSection(webFrameworkSettings).equal_range(webFrameworkName).first->second;
	size_t lastItemDefinition = vcxprojFile.rfind(vcxproj::endItemDefinitionGroupTag) + vcxproj::endItemDefinitionGroupTag.size() + 1;
	string topLevelSpaces;
	string oneBottomLevelSpaces;
	string twoBottomLevelSpaces;

	while (vcxprojFile[lastItemDefinition] != '<')
	{
		topLevelSpaces += ' ';
		lastItemDefinition++;
	}

	oneBottomLevelSpaces = topLevelSpaces + ' ';
	twoBottomLevelSpaces = oneBottomLevelSpaces + ' ';

	lastItemDefinition = vcxprojFile.rfind(vcxproj::endItemDefinitionGroupTag) + vcxproj::endItemDefinitionGroupTag.size() + 1;

	string projectReference = topLevelSpaces + vcxproj::startItemGroupTag + '\n' +
		oneBottomLevelSpaces + "<ProjectReference Include=\"..\\" + webFrameworkFolder + '\\' + webFrameworkName + '\\' + webFrameworkName + extensions::projFile + "\">\n" +
		twoBottomLevelSpaces + vcxproj::startProjectTag + webFrameworkGUID + vcxproj::endProjectTag + '\n' +
		oneBottomLevelSpaces + "</ProjectReference>\n" +
		topLevelSpaces + vcxproj::endItemGroupTag + '\n';

	vcxprojFile.insert(vcxprojFile.begin() + lastItemDefinition, projectReference.begin(), projectReference.end());
}

/*
<AdditionalLibraryDirectories>$(SolutionDir)bin\$(Configuration)-$(Platform)\WebFramework;$(SolutionDir)WebFrameworkLibrary\WebFramework\libs</AdditionalLibraryDirectories>
*/