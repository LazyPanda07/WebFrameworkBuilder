#pragma once

#include <string>
#include <array>

inline const std::string mainSectionName = "WebFramework";
inline const std::string dependenciesArrayName = "dependencies";

inline const std::string webFrameworkFolder = "WebFrameworkLibrary";

inline const std::array<std::string, 2> configurationsToDelete = { "Debug|x86 = Debug|x86", "Release|x86 = Release|x86" };

namespace extensions
{
	inline const std::string slnFile = ".sln";
	inline const std::string projFile = ".vcxproj";
}

namespace vsMacros
{
	inline const std::string solutionDir = "$(SolutionDir)";
}

namespace sln
{
	inline const std::string solutionFolderName = "Dependencies";

	inline const std::string visualCPlusPlusProjectGUID = "\"{8BC9CEB8-8B4A-11D0-8D11-00A0C91BC942}\"";
	inline const std::string solutionFolderProjectGUID = "\"{2150E333-8FDC-42A3-9474-1A3956D46DE8}\"";

	inline constexpr std::array<std::string_view, 2> buildConfigurations = { "Build.0", "ActiveCfg" };

	inline const std::string endProject = "EndProject";
	inline const std::string projectConfigurationPlatforms = "GlobalSection(ProjectConfigurationPlatforms) = postSolution";
	inline const std::string startGlobalSection = "GlobalSection";
	inline const std::string endGlobalSection = "EndGlobalSection";
	inline const std::string nestedProjects = "(NestedProjects)";
	inline const std::string postSolution = "postSolution";
	inline const std::string preSolution = "preSolution";
}

namespace vcxproj
{
	inline const std::string clCompileTag = "<ClCompile>";
	inline const std::string startAdditionalIncludeDirectoriesTag = "<AdditionalIncludeDirectories>";
	inline const std::string endAdditionalIncludeDirectoriesTag = "</AdditionalIncludeDirectories>";
}
