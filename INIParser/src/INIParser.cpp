#include "INIParser.h"

using namespace std;

namespace utility
{
	void INIParser::parse(ifstream&& file)
	{
		string sectionName;
		string tem;

		while (getline(file, tem))
		{
			size_t isStringValue = tem.find('\"');
			string key;
			string value;

			if (isStringValue == string::npos)
			{
				tem.erase(remove_if(tem.begin(), tem.end(), [](const char& c) { return isspace(c); }), tem.end());
			}
			else
			{
				tem.erase(remove_if(tem.begin(), tem.begin() + tem.find('\"'), [](const char& c) {return isspace(c); }), tem.begin() + tem.find('\"'));

				tem.erase(remove(tem.begin(), tem.end(), '\"'), tem.end());
			}

			if (tem.empty())
			{
				continue;
			}
			else
			{
				switch (tem[0])
				{
				case ';':
				case '#':
					continue;

					break;

				case '[':
					sectionName = string(tem.begin() + 1, tem.end() - 1);

					break;

				default:
					key = tem.substr(0, tem.find('='));
					value = tem.substr(tem.find('=') + 1);

					{
						size_t isArray = key.find("[]");

						if (isArray != string::npos)
						{
							key.pop_back();

							key.pop_back();
						}
					}
					

					data[sectionName].emplace(make_pair(move(key), move(value)));

					break;
				}
			}
		}
	}

	INIParser::INIParser(const string_view& fileName)
	{
		this->parse(ifstream(fileName));
	}

	INIParser::INIParser(const filesystem::path& fileName)
	{
		this->parse(ifstream(fileName));
	}

	INIParser::INIParser(ifstream&& file) noexcept
	{
		this->parse(move(file));
	}

	const INIParser::iniStructure& INIParser::get() const
	{
		return data;
	}

	const unordered_multimap<string, string>& INIParser::getSection(const string& sectionName) const
	{
		return data.find(sectionName)->second;
	}

	string INIParser::getKeyValue(const string& sectionName, const string& keyName) const
	{
		return data.find(sectionName)->second.find(keyName)->second;
	}
}