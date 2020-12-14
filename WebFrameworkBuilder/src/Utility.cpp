#include "Utility.h"

#include <iostream>

#include <Windows.h>

using namespace std;

string getGUID();

namespace utility
{
	vector<string> parseData(const string& data)
	{
		vector<string> result
		(
			[&data]() -> size_t{
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

		return data;
	}

	void modifySlnFiles(const vector<string>& slnFiles)
	{
		
	}

	void modifyVcxprojFiles(const vector<string>& vcxprojFiles)
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

		guidString = '\"' + move(guidString) + '\"';
	}
	else
	{
		throw runtime_error("Error create GUID");
	}

	return guidString;
}
