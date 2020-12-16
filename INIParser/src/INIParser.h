#pragma once

#include <unordered_map>
#include <string>
#include <filesystem>
#include <fstream>
#include <string_view>

namespace utility
{
	class INIParser
	{
	public:
		using iniStructure = std::unordered_map<std::string, std::unordered_multimap<std::string, std::string>>;	//section name - key(name=value)

	private:
		iniStructure data;

	private:
		void parse(std::ifstream&& file);

	public:
		INIParser(const std::string_view& fileName);

		INIParser(const std::filesystem::path& fileName);

		INIParser(std::ifstream&& file) noexcept;

		const iniStructure& get() const;

		const std::unordered_multimap<std::string, std::string>& getSection(const std::string& sectionName) const;

		std::string getKeyValue(const std::string& sectionName, const std::string& keyName) const;

		INIParser() = default;
	};
}