#include <iostream>
#include <fstream>
#include <string>
#include <chrono>
#include <format>
#include <vector>

using namespace std;

size_t reactPolymer(string& input, char omitLower, char omitUpper, size_t currentMin)
{
	auto polymer = vector<char>();
	polymer.reserve(currentMin);
	auto const end = prev(input.end()); // omit trailing newline
	for (auto it = input.begin(); it != end; it++)
	{
		if (*it != omitLower && *it != omitUpper)
		{
			if (!polymer.empty() && polymer.back() != *it && tolower(polymer.back()) == tolower(*it))
			{
				polymer.erase(polymer.end() - 1);
			}
			else
			{
				polymer.push_back(*it);
			}
		}

		if (polymer.size() >= currentMin)
		{
			return currentMin;
		}
	}

	return polymer.size();
}

int main()
{
	string input;
	auto readStream = ifstream("../input.txt");
	if (readStream.is_open())
	{
		string line;
		while (getline(readStream, line))
		{
			input += line;
			input.push_back('\n');
		}
	}

	const auto execStart = chrono::high_resolution_clock::now();

	/* begin solution */

	auto minLen = input.size();
	for (char l = 'a'; l <= 'z'; l++)
	{
		auto length = reactPolymer(input, l, toupper(l), minLen);
		if (length < minLen)
		{
			minLen = length;
		}
	}

	auto answer = minLen;

	/* end solution */

	const auto execEnd = chrono::high_resolution_clock::now();
	const auto micros = chrono::duration_cast<chrono::milliseconds>(execEnd - execStart);
	cout << std::format("{} ({})", answer, micros);
}